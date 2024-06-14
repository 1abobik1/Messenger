#pragma once

#include <chrono>
#include <cstddef>
#include <deque>

namespace messenger {

// Anti-spam: at most `maxEvents` events within `window`; the next one blocks the sender for `blockFor`.
class RateLimiter {
public:
    using Clock = std::chrono::steady_clock;

    RateLimiter(std::size_t maxEvents, Clock::duration window, Clock::duration blockFor);

    // Registers an attempt at `now`. Returns zero if it is allowed, otherwise how long the sender stays blocked.
    Clock::duration tryAcquire(Clock::time_point now);

private:
    std::size_t maxEvents_;
    Clock::duration window_;
    Clock::duration blockFor_;
    std::deque<Clock::time_point> events_;
    Clock::time_point blockedUntil_{};
};

}  // namespace messenger
