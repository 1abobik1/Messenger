#pragma once

#include <string>
#include <string_view>

namespace JsonObj
{
	const std::string COMMAND = "command";
	// сообщения
	const std::string MESSAGE = "message";
	// личные сообщения
	const std::string PRIVATE_MSG = "private_msg";
	// публичные сообщения
	const std::string PUBLIC_MSG = "public_msg";

	// получатель сообщения
	const std::string RECEIVER_ID = "receiver_id";
	// кто отправил сообщение
	const std::string USER_ID_FROM = "user_id_from";

	// имя пользователя
	const std::string SET_NAME = "set_name";
	const std::string NEW_NAME = "new_name";
}

namespace PathHTML
{
	// Путь к файлу HTML-клиента
	constexpr std::string_view CLIENT_HTML_FILE_PATH = "client.htm";
}

namespace ConnDB
{
	//путь к бд
	constexpr std::string_view CONNECTION_DB = "host=localhost dbname=Messenger_DB user=postgres password=Zopa_kek12";
}
