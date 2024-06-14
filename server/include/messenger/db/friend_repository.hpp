#pragma once

#include <vector>

#include "messenger/db/connection.hpp"
#include "messenger/db/models.hpp"

namespace messenger::db {

class FriendRepository {
public:
    explicit FriendRepository(Connection& connection) : connection_(connection) {}

    // Makes the two users friends with each other. Returns false if they already were.
    // The caller checks that both users exist and differ.
    bool add(UserId userId, UserId friendId);

    std::vector<UserSummary> list(UserId userId);

private:
    Connection& connection_;
};

}  // namespace messenger::db
