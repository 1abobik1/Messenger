#define _SILENCE_ALL_CXX23_DEPRECATION_ARNINGS 

#include<fstream>
#include <iostream>
#include <sstream>

#include <libpq-fe.h>
#include <uwebsockets/App.h>

#include "CommonConst.h"
#include "Server.h"

int main() {
	using namespace std;

	PGconn* connection = PQconnectdb(ConnDB::CONNECTION_DB.data());

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

	try {
		// Загрузка HTML-файл клиента
		const std::ifstream file((PathHTML::CLIENT_HTML_FILE_PATH.data()));
		if(!file.is_open())
		{
			throw std::runtime_error("Failed to open html file");
		}
		std::stringstream buffer;
		buffer << file.rdbuf();
		const std::string html_content = buffer.str();

		// Пример обработчика для запроса корневого URL и отправки HTML-клиенту
		uWS::App().get("/", [&html_content](auto* res, auto* req) {
			res->write(html_content);
			});

	}
	catch (const std::exception& e)
	{
		std::cerr << "An error occurred: " << e.what() << '\n';
		return 1;
	}

	Server server(9001);
	// запуск сервера
	server.run();

	PQfinish(connection);

	return 0;
}
