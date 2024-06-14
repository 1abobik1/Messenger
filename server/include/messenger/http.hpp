#pragma once

#include <App.h>
#include <nlohmann/json.hpp>

#include <functional>
#include <optional>
#include <string>
#include <string_view>

namespace messenger::http {

using Response = uWS::HttpResponse<false>;
using Request = uWS::HttpRequest;
using Json = nlohmann::json;

inline constexpr std::string_view kOk = "200 OK";
inline constexpr std::string_view kCreated = "201 Created";
inline constexpr std::string_view kNoContent = "204 No Content";
inline constexpr std::string_view kBadRequest = "400 Bad Request";
inline constexpr std::string_view kUnauthorized = "401 Unauthorized";
inline constexpr std::string_view kNotFound = "404 Not Found";
inline constexpr std::string_view kConflict = "409 Conflict";
inline constexpr std::string_view kPayloadTooLarge = "413 Payload Too Large";
inline constexpr std::string_view kInternalError = "500 Internal Server Error";
inline constexpr std::string_view kServiceUnavailable = "503 Service Unavailable";

// Writes responses with the CORS headers of the configured front-end origin.
// Every response is finished with exactly one of these calls.
class Responder {
public:
    explicit Responder(std::string corsOrigin) : corsOrigin_(std::move(corsOrigin)) {}

    void json(Response* res, std::string_view status, const Json& body) const;
    void error(Response* res, std::string_view status, std::string_view message) const;
    void noContent(Response* res) const;
    void preflight(Response* res) const;

    // Collects the request body and parses it as a JSON object, then calls onJson once.
    // Answers 400/413 itself for invalid or too large bodies, and 500 if onJson throws.
    void readJson(Response* res, std::function<void(Json)> onJson) const;

private:
    void writeCommonHeaders(Response* res, std::string_view status) const;

    std::string corsOrigin_;
};

// The token from "Authorization: Bearer <token>", or an empty string.
std::string bearerToken(Request* req);

// Strict non-negative integer parsing for ids from paths, queries and JSON.
std::optional<std::int64_t> parseId(std::string_view text);
std::optional<std::int64_t> jsonId(const Json& body, const char* key);
std::optional<std::string> jsonString(const Json& body, const char* key);

}  // namespace messenger::http
