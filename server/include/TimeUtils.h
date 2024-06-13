#pragma once

#include<string>

namespace time_utils {
	int getMinutes(const std::string& sent_at);
	int getSeconds(const std::string& sent_at);
}
