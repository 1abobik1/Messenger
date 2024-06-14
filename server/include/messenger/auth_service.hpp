#pragma once

#include <chrono>
#include <optional>
#include <string>
#include <string_view>
#include <variant>

#include "messenger/db/models.hpp"
#include "messenger/db/session_repository.hpp"
#include "messenger/db/user_repository.hpp"

namespace messenger {

struct Session {
    std::string token;  // given to the client once, only its SHA-256 is stored
    User user;
};

struct AuthFailure {
    enum class Reason { InvalidInput, EmailTaken, InvalidCredentials };
    Reason reason;
    std::string message;
};

using AuthOutcome = std::variant<Session, AuthFailure>;

class AuthService {
public:
    AuthService(db::UserRepository& users, db::SessionRepository& sessions, std::chrono::hours sessionTtl);

    AuthOutcome signup(const std::string& username, const std::string& email, const std::string& password);
    AuthOutcome login(const std::string& email, const std::string& password);

    // The user behind a token, if the session exists and has not expired.
    std::optional<UserId> authenticate(std::string_view token);

    void logout(std::string_view token);

private:
    Session startSession(User user);

    db::UserRepository& users_;
    db::SessionRepository& sessions_;
    std::chrono::hours sessionTtl_;
};

// 32 random bytes from the OS CSPRNG as 64 hex characters.
std::string generateToken();

}  // namespace messenger
