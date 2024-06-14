#include "messenger/db/session_repository.hpp"

namespace messenger::db {

void SessionRepository::create(const std::string& tokenHash, UserId userId, std::chrono::hours ttl) {
    connection_.exec(
        "INSERT INTO sessions (token_hash, user_id, expires_at) VALUES ($1, $2, now() + make_interval(hours "
        "=> $3))",
        {tokenHash, std::to_string(userId), std::to_string(ttl.count())});
}

std::optional<UserId> SessionRepository::findUser(const std::string& tokenHash) {
    const Result result = connection_.exec(
        "SELECT user_id FROM sessions WHERE token_hash = $1 AND expires_at > now()", {tokenHash});
    if (result.rows() == 0) {
        return std::nullopt;
    }
    return result.int64(0, 0);
}

void SessionRepository::remove(const std::string& tokenHash) {
    connection_.exec("DELETE FROM sessions WHERE token_hash = $1", {tokenHash});
}

int SessionRepository::removeExpired() {
    const Result result = connection_.exec("DELETE FROM sessions WHERE expires_at <= now() RETURNING 1");
    return result.rows();
}

}  // namespace messenger::db
