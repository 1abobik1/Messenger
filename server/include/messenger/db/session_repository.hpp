#pragma once

#include <chrono>
#include <optional>
#include <string>

#include "messenger/db/connection.hpp"
#include "messenger/db/models.hpp"

namespace messenger::db {

// Sessions are looked up by the SHA-256 (hex) of the token; the token itself is never stored.
class SessionRepository {
public:
    explicit SessionRepository(Connection& connection) : connection_(connection) {}

    void create(const std::string& tokenHash, UserId userId, std::chrono::hours ttl);

    // The user of a session that has not expired yet.
    std::optional<UserId> findUser(const std::string& tokenHash);

    void remove(const std::string& tokenHash);

    // Deletes expired sessions; returns how many were removed.
    int removeExpired();

private:
    Connection& connection_;
};

}  // namespace messenger::db
