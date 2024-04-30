#pragma once

#include <string>
#include <cstdint>

struct WebSocketUser
{
    std::uint64_t id_ = 404;
    std::string name_ = "example";
    std::string email_ = "example";
};
