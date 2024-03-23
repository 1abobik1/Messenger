#define _CRT_SECURE_NO_WARNINGS					
#define _SILENCE_ALL_CXX23_DEPRECATION_WARNINGS 


#include<fstream>

#include "server.h"


int main() {
	using namespace std;

	try
	{
		// Загрузка HTML-файл клиента
		const std::ifstream file(PathHTML::CLIENT_HTML_FILE_PATH);
		if(!file.is_open())
		{
			throw std::runtime_error("Failed to open html file");
		}
		std::stringstream buffer;
		buffer << file.rdbuf();
		const std::string html_content = buffer.str();

		// Пример обработчика для запроса корневого URL и отправки HTML-клиенту
		uWS::App().get("/", [&html_content](auto* res, auto* req) {
			res->write(html_content.data(), html_content.length(), "text/html");
			});
	}
	catch (const std::exception& e)
	{
		std::cerr << "An error occurred: " << e.what() << '\n';
		return 1;
	}

	// запуск сервера
	runServer(9001);

	return 0;
}
