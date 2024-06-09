#include "../header/TimeUtils.h"

// extracting a minute from a string like 2024-05-23 11:56:32.699288+03 - in this example the function will return 56
int time_utils::getMinutes(const std::string& sent_at)
{
    std::string minutes;
    int colon_count = 0;

    for (const char& ch : sent_at) {
        if (ch == ':') {
            colon_count++;
            continue;
        }
        else if (colon_count == 1) { 
              minutes += ch;
        }
        else if (colon_count == 2) {
            break;
        }
    }
    return std::stoi(minutes);
}

// extracting a seconds from a string like 2024-05-23 11:56:32.699288+03 - in this example the function will return 32
int time_utils::getSeconds(const std::string& sent_at)
{
    std::string seconds;
    int colon_count = 0;

    for (const char& ch : sent_at) {
        if (ch == ':') {
            colon_count++;
            continue;
        }
        else if (colon_count == 2) {
            seconds += ch;
        }
        else if (ch == '.') {
            break;
        }
    }
    return std::stoi(seconds);
}
