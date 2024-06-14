#include "messenger/validation.hpp"

#include <algorithm>
#include <cctype>
#include <cstdint>

namespace messenger::validation {

bool isValidUtf8(std::string_view text) {
    std::size_t i = 0;
    while (i < text.size()) {
        const auto byte = static_cast<unsigned char>(text[i]);
        std::size_t extra = 0;
        std::uint32_t codePoint = 0;
        if (byte < 0x80) {
            ++i;
            continue;
        } else if ((byte & 0xE0) == 0xC0) {
            extra = 1;
            codePoint = byte & 0x1F;
        } else if ((byte & 0xF0) == 0xE0) {
            extra = 2;
            codePoint = byte & 0x0F;
        } else if ((byte & 0xF8) == 0xF0) {
            extra = 3;
            codePoint = byte & 0x07;
        } else {
            return false;
        }
        if (i + extra >= text.size()) {  // truncated sequence
            return false;
        }
        for (std::size_t k = 1; k <= extra; ++k) {
            const auto next = static_cast<unsigned char>(text[i + k]);
            if ((next & 0xC0) != 0x80) {
                return false;
            }
            codePoint = (codePoint << 6) | (next & 0x3F);
        }
        // Reject overlong encodings, surrogates and values beyond U+10FFFF.
        const bool overlong = (extra == 1 && codePoint < 0x80) || (extra == 2 && codePoint < 0x800) ||
                              (extra == 3 && codePoint < 0x10000);
        if (overlong || (codePoint >= 0xD800 && codePoint <= 0xDFFF) || codePoint > 0x10FFFF) {
            return false;
        }
        i += extra + 1;
    }
    return true;
}

std::size_t utf8Length(std::string_view text) {
    return static_cast<std::size_t>(std::count_if(
        text.begin(), text.end(), [](char c) { return (static_cast<unsigned char>(c) & 0xC0) != 0x80; }));
}

std::string trim(std::string_view text) {
    const auto isSpace = [](char c) { return std::isspace(static_cast<unsigned char>(c)) != 0; };
    const auto begin = std::find_if_not(text.begin(), text.end(), isSpace);
    const auto end =
        std::find_if_not(text.rbegin(), std::string_view::reverse_iterator(begin), isSpace).base();
    return std::string(begin, end);
}

std::string normalizeEmail(std::string_view email) {
    std::string result = trim(email);
    std::transform(result.begin(), result.end(), result.begin(),
                   [](char c) { return static_cast<char>(std::tolower(static_cast<unsigned char>(c))); });
    return result;
}

std::optional<std::string> validateUsername(std::string_view username) {
    if (!isValidUtf8(username)) {
        return "username must be valid UTF-8";
    }
    const std::string trimmed = trim(username);
    const std::size_t length = utf8Length(trimmed);
    if (length == 0 || length > kMaxUsernameLength) {
        return "username must be 1-20 characters long";
    }
    if (std::all_of(trimmed.begin(), trimmed.end(),
                    [](char c) { return std::isdigit(static_cast<unsigned char>(c)); })) {
        return "username must not consist of digits only";
    }
    return std::nullopt;
}

std::optional<std::string> validateEmail(std::string_view email) {
    const std::string normalized = normalizeEmail(email);
    const std::size_t at = normalized.find('@');
    const bool shapeOk = normalized.size() <= 254 && at != std::string::npos && at > 0 &&
                         normalized.find('@', at + 1) == std::string::npos &&
                         normalized.find('.', at + 2) != std::string::npos && normalized.back() != '.' &&
                         std::none_of(normalized.begin(), normalized.end(), [](char c) {
                             return std::isspace(static_cast<unsigned char>(c)) != 0;
                         });
    if (!shapeOk || !isValidUtf8(normalized)) {
        return "invalid email";
    }
    return std::nullopt;
}

std::optional<std::string> validatePassword(std::string_view password) {
    if (!isValidUtf8(password)) {
        return "password must be valid UTF-8";
    }
    if (utf8Length(password) < kMinPasswordLength) {
        return "password must be at least 10 characters long";
    }
    if (password.size() > kMaxPasswordBytes) {
        return "password must be at most 72 bytes long";
    }
    return std::nullopt;
}

std::optional<std::string> validateMessageBody(std::string_view body) {
    if (!isValidUtf8(body)) {
        return "message must be valid UTF-8";
    }
    if (trim(body).empty()) {
        return "message must not be empty";
    }
    if (utf8Length(body) > kMaxMessageLength) {
        return "message must be at most 4000 characters long";
    }
    return std::nullopt;
}

}  // namespace messenger::validation
