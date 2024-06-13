#include <exception>
#include <iostream>

#include "CommonConst.h"
#include "Server.h"
#include "messenger/config.hpp"

int main() {
	try {
		const messenger::Config config = messenger::loadConfigFromEnv();
		PathDB::CONNECTION_DB = config.databaseUrl;

		Server server(config.port);
		server.run();
	}
	catch (const std::exception& e) {
		std::cerr << "Fatal: " << e.what() << '\n';
		return 1;
	}
	return 0;
}
