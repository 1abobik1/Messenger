#include "messenger/chat.hpp"

#include <chrono>

#include "messenger/log.hpp"
#include "messenger/validation.hpp"

namespace messenger {

using http::Json;

namespace {

// 8 messages per 10 seconds; the 9th blocks the sender for 30 seconds.
constexpr std::size_t kMaxMessagesPerWindow = 8;
constexpr std::chrono::seconds kRateWindow(10);
constexpr std::chrono::seconds kBlockDuration(30);

constexpr unsigned kMaxPayloadBytes = 16 * 1024;
constexpr unsigned short kIdleTimeoutSeconds = 120;

void sendError(ChatSocket* ws, std::string_view code, std::string_view message, Json extra = Json::object()) {
    extra["type"] = "error";
    extra["code"] = code;
    extra["message"] = message;
    ws->send(extra.dump(), uWS::OpCode::TEXT);
}

}  // namespace

std::string userTopic(UserId userId) {
    return "user:" + std::to_string(userId);
}

void ChatHandler::registerOn(uWS::App& app) {
    app.ws<SocketData>(
        "/ws",
        {
            .maxPayloadLength = kMaxPayloadBytes,
            .idleTimeout = kIdleTimeoutSeconds,
            .upgrade =
                [this](uWS::HttpResponse<false>* res, uWS::HttpRequest* req, us_socket_context_t* context) {
                    std::optional<UserId> userId;
                    try {
                        userId = services_.auth.authenticate(req->getQuery("token"));
                    } catch (const std::exception& e) {
                        log::error(std::string("websocket auth failed: ") + e.what());
                    }
                    if (!userId) {
                        res->writeStatus(http::kUnauthorized)->end("authentication required");
                        return;
                    }
                    res->upgrade<SocketData>(SocketData{*userId}, req->getHeader("sec-websocket-key"),
                                             req->getHeader("sec-websocket-protocol"),
                                             req->getHeader("sec-websocket-extensions"), context);
                },
            .open =
                [](ChatSocket* ws) {
                    ws->subscribe(userTopic(ws->getUserData()->userId));
                    log::info("user " + std::to_string(ws->getUserData()->userId) + " connected");
                },
            .message = [this](ChatSocket* ws, std::string_view raw, uWS::OpCode) { onMessage(ws, raw); },
            .close =
                [](ChatSocket* ws, int, std::string_view) {
                    log::info("user " + std::to_string(ws->getUserData()->userId) + " disconnected");
                },
        });
}

void ChatHandler::onMessage(ChatSocket* ws, std::string_view raw) {
    const Json request = Json::parse(raw, nullptr, false);
    if (request.is_discarded() || !request.is_object()) {
        sendError(ws, "bad_request", "message must be a JSON object");
        return;
    }
    const auto type = http::jsonString(request, "type");
    if (type != "message") {
        sendError(ws, "unknown_type", "unknown message type");
        return;
    }
    const auto to = http::jsonId(request, "to");
    const auto body = http::jsonString(request, "body");
    if (!to || !body) {
        sendError(ws, "bad_request", "'to' (user id) and 'body' (string) are required");
        return;
    }

    try {
        sendMessage(ws, *to, *body);
    } catch (const std::exception& e) {
        log::error(std::string("sending a message failed: ") + e.what());
        sendError(ws, "internal_error", "the message was not sent, try again");
    }
}

void ChatHandler::sendMessage(ChatSocket* ws, UserId to, const std::string& body) {
    const UserId from = ws->getUserData()->userId;
    if (to == from) {
        sendError(ws, "bad_request", "you cannot send a message to yourself");
        return;
    }
    if (const auto error = validation::validateMessageBody(body)) {
        sendError(ws, "bad_request", *error);
        return;
    }

    if (!services_.users.findById(to)) {
        sendError(ws, "not_found", "recipient not found");
        return;
    }

    // Only messages that would really be sent count; checked before storing, so spam never reaches the
    // database.
    const auto blockedFor = limiterFor(from).tryAcquire(RateLimiter::Clock::now());
    if (blockedFor > RateLimiter::Clock::duration::zero()) {
        const auto seconds = std::chrono::ceil<std::chrono::seconds>(blockedFor).count();
        sendError(ws, "rate_limited", "too many messages, you are blocked for a while",
                  {{"retry_after", seconds}});
        return;
    }

    const Message message = services_.messages.insert(from, to, body);
    const std::string payload = Json{{"type", "message"}, {"message", toJson(message)}}.dump();

    ws->publish(userTopic(to), payload, uWS::OpCode::TEXT);  // the recipient's tabs
    ws->publish(userTopic(from), payload,
                uWS::OpCode::TEXT);        // the sender's other tabs (publish skips ws itself)
    ws->send(payload, uWS::OpCode::TEXT);  // confirmation with the stored id and time
}

RateLimiter& ChatHandler::limiterFor(UserId userId) {
    return limiters_.try_emplace(userId, kMaxMessagesPerWindow, kRateWindow, kBlockDuration).first->second;
}

}  // namespace messenger
