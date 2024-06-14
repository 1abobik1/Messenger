#pragma once

#include <optional>
#include <string>

#include "messenger/db/connection.hpp"
#include "messenger/db/models.hpp"

namespace messenger::db {

class UserRepository {
public:
    explicit UserRepository(Connection& connection) : connection_(connection) {}

    // Returns std::nullopt if the email is already taken.
    std::optional<User> create(const std::string& username, const std::string& email,
                               const std::string& passwordHash);

    std::optional<User> findByEmail(const std::string& email);
    std::optional<User> findById(UserId id);

private:
    Connection& connection_;
};

}  // namespace messenger::db
