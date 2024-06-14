#include <exception>
#include <string>

#include "messenger/config.hpp"
#include "messenger/log.hpp"
#include "messenger/server.hpp"

int main() {
    try {
        return messenger::runServer(messenger::loadConfigFromEnv());
    } catch (const std::exception& e) {
        messenger::log::error(std::string("fatal: ") + e.what());
        return 1;
    }
}
