#pragma once

#include <App.h>

#include <string>
#include <string_view>
#include <unordered_map>

#include "messenger/rate_limiter.hpp"
#include "messenger/services.hpp"

namespace messenger {

struct SocketData {
    UserId userId = 0;
};

using ChatSocket = uWS::WebSocket<false, true, SocketData>;

// WebSocket endpoint /ws?token=<session token>.
//
// Client -> server: {"type":"message","to":<user id>,"body":"..."}
// Server -> client: {"type":"message","message":{id,from,to,body,sent_at}}  (to both users and all their tabs)
//                   {"type":"error","code":"...","message":"...","retry_after":<seconds, for rate_limited>}
class ChatHandler {
public:
    explicit ChatHandler(Services& services) : services_(services) {}

    void registerOn(uWS::App& app);

private:
    void onMessage(ChatSocket* ws, std::string_view raw);
    void sendMessage(ChatSocket* ws, UserId to, const std::string& body);
    RateLimiter& limiterFor(UserId userId);

    Services& services_;
    // Per user rather than per socket, so opening more tabs does not raise the limit.
    std::unordered_map<UserId, RateLimiter> limiters_;
};

// Topic every socket of a user is subscribed to.
std::string userTopic(UserId userId);

}  // namespace messenger
