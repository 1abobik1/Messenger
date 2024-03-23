#pragma once

#include <string>

namespace JsonObj
{
	const std::string COMMAND = "command";
	// сообщени€
	const std::string MESSAGE = "message";
	// личные сообщени€
	const std::string PRIVATE_MSG = "private_msg";
	// публичные сообщени€
	const std::string PUBLIC_MSG = "public_msg";

	// получатель сообщени€
	const std::string RECEIVER_ID = "receiver_id";
	// кто отправил сообщение
	const std::string USER_ID_FROM = "user_id_from";

	// им€ пользовател€
	const std::string SET_NAME = "set_name";
	const std::string NEW_NAME = "new_name";
}

// ѕуть к файлу HTML-клиента
namespace PathHTML
{
	const std::string CLIENT_HTML_FILE_PATH = "client.htm";
}
