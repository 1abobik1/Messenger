#pragma once

#include <string>
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <sstream>

class ServiceFunc
{
protected:
	ServiceFunc() = default;
	~ServiceFunc() = default;

public:

	static std::string UrlDecode(const std::string& urlEncodedString);

	//function for checking data integrity
	static uint32_t crc32(const char* s, size_t n, uint32_t crc = 0xFFFFFFFF);
};
