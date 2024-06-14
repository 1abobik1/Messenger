#pragma once

#include <string>
#include <string_view>

namespace messenger {

// SHA-256 of the input as 64 lowercase hex characters. Used to store session tokens without OpenSSL.
std::string sha256Hex(std::string_view data);

}  // namespace messenger
