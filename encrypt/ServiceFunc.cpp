#include "ServiceFunc.h"

#include <regex>


std::string ServiceFunc::UrlDecode(const std::string& urlEncodedString)
{
	const std::regex pattern("%22");
    return std::regex_replace(urlEncodedString, pattern, "\"");
}

uint32_t ServiceFunc::crc32(const char* s, size_t n, uint32_t crc)
{
    for (size_t i = 0; i < n; ++i) {
        unsigned char ch = static_cast<unsigned char>(s[i]);
        for (size_t j = 0; j < 8; ++j) {
            uint32_t b = (ch ^ crc) & 1;
            crc >>= 1;
            if (b) crc = crc ^ 0xEDB88320;
            ch >>= 1;
        }
    }

    return crc;
}
