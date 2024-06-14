#include "messenger/rate_limiter.hpp"

namespace messenger {

RateLimiter::RateLimiter(std::size_t maxEvents, Clock::duration window, Clock::duration blockFor)
    : maxEvents_(maxEvents), window_(window), blockFor_(blockFor) {}

RateLimiter::Clock::duration RateLimiter::tryAcquire(Clock::time_point now) {
    if (now < blockedUntil_) {
        return blockedUntil_ - now;
    }
    while (!events_.empty() && now - events_.front() >= window_) {
        events_.pop_front();
    }
    if (events_.size() >= maxEvents_) {
        events_.clear();
        blockedUntil_ = now + blockFor_;
        return blockFor_;
    }
    events_.push_back(now);
    return Clock::duration::zero();
}

}  // namespace messenger
