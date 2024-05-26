#pragma once

#include <string>
#include <cstdint>
#include <deque>
#include <chrono>

struct WebSocketUser
{
    std::string email_sender_id_;
    std::uint64_t receiver_id_;
    std::uint64_t sender_id_; // user_id
    std::string message_;

    std::string sent_at;
    std::deque<std::pair<int, int>> message_times;
    bool is_blocked = false;
    std::chrono::steady_clock::time_point unblock_time;
};
