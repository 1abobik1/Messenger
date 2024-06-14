#include "messenger/http.hpp"

#include <charconv>
#include <memory>

#include "messenger/log.hpp"

namespace messenger::http {

namespace {
constexpr std::size_t kMaxBodyBytes = 64 * 1024;
}

void Responder::writeCommonHeaders(Response* res, std::string_view status) const {
    res->writeStatus(status);
    res->writeHeader("Access-Control-Allow-Origin", corsOrigin_);
    res->writeHeader("Vary", "Origin");
}

void Responder::json(Response* res, std::string_view status, const Json& body) const {
    writeCommonHeaders(res, status);
    res->writeHeader("Content-Type", "application/json; charset=utf-8");
    res->end(body.dump());
}

void Responder::error(Response* res, std::string_view status, std::string_view message) const {
    json(res, status, Json{{"error", message}});
}

void Responder::noContent(Response* res) const {
    writeCommonHeaders(res, kNoContent);
    res->end();
}

void Responder::preflight(Response* res) const {
    writeCommonHeaders(res, kNoContent);
    res->writeHeader("Access-Control-Allow-Headers", "Content-Type, Authorization");
    res->writeHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    res->writeHeader("Access-Control-Max-Age", "600");
    res->end();
}

void Responder::readJson(Response* res, std::function<void(Json)> onJson) const {
    struct State {
        std::string body;
        bool aborted = false;
        bool finished = false;
    };
    // Captured by value in both callbacks: the handler returns before the body arrives.
    auto state = std::make_shared<State>();

    res->onAborted([state] { state->aborted = true; });
    res->onData([this, res, state, onJson = std::move(onJson)](std::string_view chunk, bool isLast) mutable {
        if (state->aborted || state->finished) {
            return;
        }
        if (state->body.size() + chunk.size() > kMaxBodyBytes) {
            state->finished = true;
            error(res, kPayloadTooLarge, "request body is too large");
            return;
        }
        state->body.append(chunk);
        // The last chunk may be empty, so this check must not depend on chunk.size().
        if (!isLast) {
            return;
        }
        state->finished = true;

        Json body = Json::parse(state->body, nullptr, false);
        if (body.is_discarded() || !body.is_object()) {
            error(res, kBadRequest, "request body must be a JSON object");
            return;
        }
        try {
            onJson(std::move(body));
        } catch (const std::exception& e) {
            log::error(std::string("request failed: ") + e.what());
            error(res, kInternalError, "internal server error");
        }
    });
}

std::string bearerToken(Request* req) {
    constexpr std::string_view kPrefix = "Bearer ";
    const std::string_view header = req->getHeader("authorization");
    if (header.size() <= kPrefix.size() || header.substr(0, kPrefix.size()) != kPrefix) {
        return {};
    }
    return std::string(header.substr(kPrefix.size()));
}

std::optional<std::int64_t> parseId(std::string_view text) {
    std::int64_t value = 0;
    const auto [ptr, ec] = std::from_chars(text.data(), text.data() + text.size(), value);
    if (text.empty() || ec != std::errc() || ptr != text.data() + text.size() || value <= 0) {
        return std::nullopt;
    }
    return value;
}

std::optional<std::int64_t> jsonId(const Json& body, const char* key) {
    const auto it = body.find(key);
    if (it == body.end() || !it->is_number_integer() || it->get<std::int64_t>() <= 0) {
        return std::nullopt;
    }
    return it->get<std::int64_t>();
}

std::optional<std::string> jsonString(const Json& body, const char* key) {
    const auto it = body.find(key);
    if (it == body.end() || !it->is_string()) {
        return std::nullopt;
    }
    return it->get<std::string>();
}

}  // namespace messenger::http
