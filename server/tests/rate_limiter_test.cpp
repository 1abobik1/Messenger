#include "messenger/rate_limiter.hpp"

#include <gtest/gtest.h>

using messenger::RateLimiter;
using namespace std::chrono_literals;

namespace {
const RateLimiter::Clock::time_point kStart{};
constexpr auto kZero = RateLimiter::Clock::duration::zero();
}  // namespace

TEST(RateLimiter, AllowsUpToTheLimitWithinTheWindow) {
    RateLimiter limiter(3, 10s, 30s);
    EXPECT_EQ(limiter.tryAcquire(kStart), kZero);
    EXPECT_EQ(limiter.tryAcquire(kStart + 1s), kZero);
    EXPECT_EQ(limiter.tryAcquire(kStart + 2s), kZero);
}

TEST(RateLimiter, BlocksTheNextEventAndReportsTheRemainingTime) {
    RateLimiter limiter(3, 10s, 30s);
    for (int i = 0; i < 3; ++i) {
        limiter.tryAcquire(kStart + std::chrono::seconds(i));
    }
    EXPECT_EQ(limiter.tryAcquire(kStart + 3s), 30s);
    EXPECT_EQ(limiter.tryAcquire(kStart + 13s), 20s);
    EXPECT_EQ(limiter.tryAcquire(kStart + 33s), kZero);  // unblocked, counting starts over
}

TEST(RateLimiter, OldEventsLeaveTheWindow) {
    RateLimiter limiter(2, 10s, 30s);
    EXPECT_EQ(limiter.tryAcquire(kStart), kZero);
    EXPECT_EQ(limiter.tryAcquire(kStart + 5s), kZero);
    EXPECT_EQ(limiter.tryAcquire(kStart + 10s), kZero);  // the first one is exactly 10 s old
    EXPECT_EQ(limiter.tryAcquire(kStart + 11s), 30s);
}

TEST(RateLimiter, WorksAcrossMinuteBoundaries) {
    // The old implementation compared the minute and second fields of timestamps and broke at hh:mm:59 ->
    // hh:mm+1:00.
    RateLimiter limiter(2, 10s, 30s);
    const auto almostNextMinute = kStart + 59s;
    EXPECT_EQ(limiter.tryAcquire(almostNextMinute), kZero);
    EXPECT_EQ(limiter.tryAcquire(almostNextMinute + 1s), kZero);
    EXPECT_EQ(limiter.tryAcquire(almostNextMinute + 2s), 30s);
}
