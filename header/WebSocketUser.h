#pragma once

#include <string>
#include <cstdint>

struct WebSocketUser
{
    std::string email_sender_id_;
    std::uint64_t receiver_id_;
    std::uint64_t sender_id_;
    std::string message_;
};
