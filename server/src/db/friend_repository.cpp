#include "messenger/db/friend_repository.hpp"

#include <string>

namespace messenger::db {

bool FriendRepository::add(UserId userId, UserId friendId) {
    const Result result = connection_.exec(
        "INSERT INTO friendships (user_id, friend_id) VALUES ($1, $2), ($2, $1) "
        "ON CONFLICT DO NOTHING RETURNING 1",
        {std::to_string(userId), std::to_string(friendId)});
    return result.rows() > 0;
}

std::vector<UserSummary> FriendRepository::list(UserId userId) {
    const Result result = connection_.exec(
        "SELECT u.id, u.username FROM friendships f JOIN users u ON u.id = f.friend_id "
        "WHERE f.user_id = $1 ORDER BY lower(u.username), u.id",
        {std::to_string(userId)});
    std::vector<UserSummary> friends;
    friends.reserve(static_cast<std::size_t>(result.rows()));
    for (int row = 0; row < result.rows(); ++row) {
        friends.push_back({result.int64(row, 0), result.text(row, 1)});
    }
    return friends;
}

}  // namespace messenger::db
