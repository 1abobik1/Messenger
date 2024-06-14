#pragma once

#include <cstdint>
#include <string>

namespace messenger {

using UserId = std::int64_t;
using MessageId = std::int64_t;

struct User {
    UserId id = 0;
    std::string username;
    std::string email;
    std::string passwordHash;
};

// What other users are allowed to see about a user.
struct UserSummary {
    UserId id = 0;
    std::string username;
};

struct Message {
    MessageId id = 0;
    UserId from = 0;
    UserId to = 0;
    std::string body;
    std::string sentAt;  // ISO 8601 in UTC, e.g. 2024-06-13T10:00:00.123Z
};

}  // namespace messenger
