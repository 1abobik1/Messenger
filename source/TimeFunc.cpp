#include "../header/TimeFunc.h"


int time_helper::helperGetMinutes(const std::string& sent_at)
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

int time_helper::helperGetSeconds(const std::string& sent_at)
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
