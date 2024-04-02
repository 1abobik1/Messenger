#pragma once

#include <string>
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <sstream>

class Encrypt
{
protected:
	Encrypt() = default;
	~Encrypt() = default;

public:

	static std::string UrlDecode(const std::string& urlEncodedString);

};
