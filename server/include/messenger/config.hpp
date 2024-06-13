#pragma once

#include <chrono>
#include <cstdint>
#include <string>

namespace messenger {

struct Config {
    std::uint16_t port = 9000;
    std::string databaseUrl;                          // libpq conninfo or postgres:// URL
    std::string corsOrigin = "http://localhost:3000";  // the front-end origin allowed to call the API
    std::chrono::hours sessionTtl{24 * 7};
};

// Reads MESSENGER_PORT, DATABASE_URL, CORS_ORIGIN, SESSION_TTL_HOURS.
// Throws std::runtime_error with a readable message if a value is missing or invalid.
Config loadConfigFromEnv();

}  // namespace messenger
