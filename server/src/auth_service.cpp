#include "messenger/auth_service.hpp"

#include <random>

#include "bcrypt.h"
#include "messenger/sha256.hpp"
#include "messenger/validation.hpp"

namespace messenger {

namespace {

constexpr unsigned kBcryptRounds = 10;
constexpr std::size_t kTokenBytes = 32;
constexpr std::size_t kTokenHexLength = kTokenBytes * 2;

AuthFailure invalidInput(std::string message) {
    return {AuthFailure::Reason::InvalidInput, std::move(message)};
}

AuthFailure invalidCredentials() {
    // The same message for a wrong email and a wrong password: do not reveal which accounts exist.
    return {AuthFailure::Reason::InvalidCredentials, "wrong email or password"};
}

}  // namespace

std::string generateToken() {
    static std::random_device device;
    static constexpr char kHex[] = "0123456789abcdef";
    std::string token;
    token.reserve(kTokenHexLength);
    for (std::size_t i = 0; i < kTokenBytes; ++i) {
        const auto byte = static_cast<unsigned char>(device() & 0xFF);
        token.push_back(kHex[byte >> 4]);
        token.push_back(kHex[byte & 0xF]);
    }
    return token;
}

AuthService::AuthService(db::UserRepository& users, db::SessionRepository& sessions,
                         std::chrono::hours sessionTtl)
    : users_(users), sessions_(sessions), sessionTtl_(sessionTtl) {}

AuthOutcome AuthService::signup(const std::string& username, const std::string& email,
                                const std::string& password) {
    if (auto error = validation::validateUsername(username)) {
        return invalidInput(*error);
    }
    if (auto error = validation::validateEmail(email)) {
        return invalidInput(*error);
    }
    if (auto error = validation::validatePassword(password)) {
        return invalidInput(*error);
    }

    const std::string hash = bcrypt::generateHash(password, kBcryptRounds);
    auto user = users_.create(validation::trim(username), validation::normalizeEmail(email), hash);
    if (!user) {
        return AuthFailure{AuthFailure::Reason::EmailTaken, "a user with this email already exists"};
    }
    return startSession(std::move(*user));
}

AuthOutcome AuthService::login(const std::string& email, const std::string& password) {
    if (validation::validateEmail(email) || password.empty() ||
        password.size() > validation::kMaxPasswordBytes) {
        return invalidCredentials();
    }

    const auto user = users_.findByEmail(validation::normalizeEmail(email));
    if (!user) {
        // Spend the same time as for an existing user, so response time does not reveal registered emails.
        static const std::string kDummyHash = bcrypt::generateHash("dummy-password", kBcryptRounds);
        bcrypt::validatePassword(password, kDummyHash);
        return invalidCredentials();
    }
    if (!bcrypt::validatePassword(password, user->passwordHash)) {
        return invalidCredentials();
    }
    return startSession(*user);
}

std::optional<UserId> AuthService::authenticate(std::string_view token) {
    if (token.size() != kTokenHexLength) {
        return std::nullopt;
    }
    return sessions_.findUser(sha256Hex(token));
}

void AuthService::logout(std::string_view token) {
    if (token.size() == kTokenHexLength) {
        sessions_.remove(sha256Hex(token));
    }
}

Session AuthService::startSession(User user) {
    std::string token = generateToken();
    sessions_.create(sha256Hex(token), user.id, sessionTtl_);
    return Session{std::move(token), std::move(user)};
}

}  // namespace messenger
