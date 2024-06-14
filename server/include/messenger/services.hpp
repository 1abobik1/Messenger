#pragma once

#include "messenger/auth_service.hpp"
#include "messenger/db/connection.hpp"
#include "messenger/db/friend_repository.hpp"
#include "messenger/db/message_repository.hpp"
#include "messenger/db/user_repository.hpp"
#include "messenger/http.hpp"

namespace messenger {

// Everything the HTTP and WebSocket handlers need. Owned by runServer(); handlers keep references.
struct Services {
    db::Connection& connection;
    db::UserRepository& users;
    db::FriendRepository& friends;
    db::MessageRepository& messages;
    AuthService& auth;
    http::Responder& responder;
};

nlohmann::json toJson(const UserSummary& user);
nlohmann::json toJson(const Message& message);

}  // namespace messenger
