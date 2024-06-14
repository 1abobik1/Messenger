#pragma once

#include <string_view>

// Minimal logger: "2024-06-14T10:00:00Z INFO  message" to stderr. Never log passwords or tokens.
namespace messenger::log {

void info(std::string_view message);
void warn(std::string_view message);
void error(std::string_view message);

}  // namespace messenger::log
