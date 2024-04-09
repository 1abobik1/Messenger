#define _SILENCE_ALL_CXX23_DEPRECATION_ARNINGS 
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>

#include <libpq-fe.h>

#include "header/CommonConst.h"
#include "header/Server.h"

int main() {
	PGconn* connection = PQconnectdb(PathDB::CONNECTION_DB.data());

	try
	{
		if(PQstatus(connection) != CONNECTION_OK)
		{
			throw std::runtime_error(PQerrorMessage(connection));
		}

	}
	catch (const std::exception& e)
	{
		std::cerr << "Database error: " << e.what() << '\n';
		PQfinish(connection);
		return 1;
	}

	std::cout << "Connected to database!" << '\n';

	Server server(9001);

	server.run();

	PQfinish(connection);

	return 0;
}
