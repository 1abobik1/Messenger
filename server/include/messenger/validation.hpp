#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <string_view>

// Input checks shared by the HTTP API and the chat. Each validate* returns an error message, or nullopt if
// OK.
namespace messenger::validation {

inline constexpr std::size_t kMaxUsernameLength = 20;
inline constexpr std::size_t kMinPasswordLength = 10;
inline constexpr std::size_t kMaxPasswordBytes = 72;  // bcrypt ignores everything after 72 bytes
inline constexpr std::size_t kMaxMessageLength = 4000;

bool isValidUtf8(std::string_view text);

// Number of Unicode code points; the input must be valid UTF-8.
std::size_t utf8Length(std::string_view text);

std::string trim(std::string_view text);

// Trims and lowercases ASCII letters, so "  Bob@Mail.RU " and "bob@mail.ru" are the same account.
std::string normalizeEmail(std::string_view email);

std::optional<std::string> validateUsername(std::string_view username);
std::optional<std::string> validateEmail(std::string_view email);
std::optional<std::string> validatePassword(std::string_view password);
std::optional<std::string> validateMessageBody(std::string_view body);

}  // namespace messenger::validation
