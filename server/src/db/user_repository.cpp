#include "messenger/db/user_repository.hpp"

namespace messenger::db {

namespace {
User userFromRow(const Result& result, int row) {
    return User{result.int64(row, 0), result.text(row, 1), result.text(row, 2), result.text(row, 3)};
}
}  // namespace

std::optional<User> UserRepository::create(const std::string& username, const std::string& email,
                                           const std::string& passwordHash) {
    try {
        const Result result = connection_.exec(
            "INSERT INTO users (username, email, password_hash) VALUES ($1, $2, $3) "
            "RETURNING id, username, email, password_hash",
            {username, email, passwordHash});
        return userFromRow(result, 0);
    } catch (const DbError& e) {
        if (e.isUniqueViolation()) {
            return std::nullopt;
        }
        throw;
    }
}

std::optional<User> UserRepository::findByEmail(const std::string& email) {
    const Result result = connection_.exec(
        "SELECT id, username, email, password_hash FROM users WHERE lower(email) = lower($1)", {email});
    if (result.rows() == 0) {
        return std::nullopt;
    }
    return userFromRow(result, 0);
}

std::optional<UserSummary> UserRepository::findById(UserId id) {
    const Result result = connection_.exec("SELECT id, username FROM users WHERE id = $1", {std::to_string(id)});
    if (result.rows() == 0) {
        return std::nullopt;
    }
    return UserSummary{result.int64(0, 0), result.text(0, 1)};
}

}  // namespace messenger::db
