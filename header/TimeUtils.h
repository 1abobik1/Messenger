#pragma once

#include<string>

namespace time_helper {
	int helperGetMinutes(const std::string& sent_at);
	int helperGetSeconds(const std::string& sent_at);
}
