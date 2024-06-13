#pragma once

#include <string>
#include <cstdint>
#include <deque>
#include <chrono>

struct WebSocketUser
{
    std::string email_sender_id_; // email of the user who sent the message
    std::uint64_t receiver_id_; // ID of the user to whom the message was sent
    std::uint64_t sender_id_; // ID of the user who sent the message
    std::string message_; // message text
    std::string sent_at; // time when the message was sent
    std::deque<std::pair<int, int>> message_times; // data structure for tracking over what period of time the first 8 messages were sent (needed to detect spam messages)
    bool is_blocked = false; // flag indicating that the user is blocked from sending messages
    std::chrono::steady_clock::time_point unblock_time; // time when the user will be unblocked
};
