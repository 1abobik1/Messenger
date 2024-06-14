#include "messenger/log.hpp"

#include <chrono>
#include <ctime>
#include <iostream>

namespace messenger::log {

namespace {

void write(std::string_view level, std::string_view message) {
    const std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::tm utc{};
#ifdef _WIN32
    gmtime_s(&utc, &now);
#else
    gmtime_r(&now, &utc);
#endif
    char timestamp[32];
    std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%SZ", &utc);
    std::cerr << timestamp << ' ' << level << ' ' << message << '\n';
}

}  // namespace

void info(std::string_view message) {
    write("INFO ", message);
}
void warn(std::string_view message) {
    write("WARN ", message);
}
void error(std::string_view message) {
    write("ERROR", message);
}

}  // namespace messenger::log
