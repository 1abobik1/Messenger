#include "Encrypt.h"

#include <regex>


std::string Encrypt::UrlDecode(const std::string& urlEncodedString)
{
    std::regex pattern("%22");
    return std::regex_replace(urlEncodedString, pattern, "\"");
}
