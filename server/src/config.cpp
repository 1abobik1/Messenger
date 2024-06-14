#include "messenger/config.hpp"

#include <charconv>
#include <cstdlib>
#include <stdexcept>
#include <string_view>

namespace messenger {

namespace {

std::string_view env(const char* name) {
    const char* value = std::getenv(name);
    return value == nullptr ? std::string_view() : std::string_view(value);
}

long parsePositive(std::string_view text, const char* name, long max) {
    long value = 0;
    const auto [ptr, ec] = std::from_chars(text.data(), text.data() + text.size(), value);
    if (ec != std::errc() || ptr != text.data() + text.size() || value <= 0 || value > max) {
        throw std::runtime_error(std::string(name) + " must be an integer in 1.." + std::to_string(max));
    }
    return value;
}

}  // namespace

Config loadConfigFromEnv() {
    Config config;

    config.databaseUrl = std::string(env("DATABASE_URL"));
    if (config.databaseUrl.empty()) {
        throw std::runtime_error(
            "DATABASE_URL is not set, e.g. "
            "DATABASE_URL=postgresql://messenger:messenger@localhost:5432/messenger");
    }
    if (const auto port = env("MESSENGER_PORT"); !port.empty()) {
        config.port = static_cast<std::uint16_t>(parsePositive(port, "MESSENGER_PORT", 65535));
    }
    if (const auto origin = env("CORS_ORIGIN"); !origin.empty()) {
        config.corsOrigin = std::string(origin);
    }
    if (const auto ttl = env("SESSION_TTL_HOURS"); !ttl.empty()) {
        config.sessionTtl = std::chrono::hours(parsePositive(ttl, "SESSION_TTL_HOURS", 24 * 365));
    }
    return config;
}

}  // namespace messenger
