#include "../header/TimeUtils.h"

std::string time_utils::ExtractTime(const std::string& sent_at)
{
	if (const size_t space_pos = sent_at.find(' '); space_pos != std::string::npos && space_pos + 1 < sent_at.size()) {
		return sent_at.substr(space_pos + 1, 5);
	}
	return "";
}