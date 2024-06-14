#pragma once

#include <optional>
#include <string>
#include <vector>

#include "messenger/db/connection.hpp"
#include "messenger/db/models.hpp"

namespace messenger::db {

class MessageRepository {
public:
    explicit MessageRepository(Connection& connection) : connection_(connection) {}

    Message insert(UserId from, UserId to, const std::string& body);

    // Messages between two users, newest first. beforeId pages back through older messages.
    std::vector<Message> conversation(UserId userId, UserId peerId, std::optional<MessageId> beforeId,
                                      int limit);

private:
    Connection& connection_;
};

}  // namespace messenger::db
