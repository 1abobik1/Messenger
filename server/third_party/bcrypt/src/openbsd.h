#ifndef ARC4RANDOM_H_INCLUDED
#define ARC4RANDOM_H_INCLUDED

// Modified for Messenger: the upstream version fills the salt with rand() seeded by time(NULL),
// so two hashes made within the same second got the same salt. std::random_device is backed by
// the OS CSPRNG on Linux (getrandom), macOS and Windows (BCryptGenRandom).

#include <cstddef>
#include <random>

inline void arc4random_buf(void *buf, size_t nbytes)
{
    static std::random_device device;
    for (size_t n = 0; n < nbytes; ++n)
        static_cast<unsigned char *>(buf)[n] = static_cast<unsigned char>(device() & 0xFF);
}

inline void arc4random_init(void) {}

#endif // ARC4RANDOM_H_INCLUDED
