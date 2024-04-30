#define _CRT_SECURE_NO_WARNINGS

#include "../header/MessagerHandler.h"
#include <iostream>

#include "../header/Server.h"
#include "../header/CommonConst.h"

using namespace JsonChat;


void MessagerHandler::ConnectedUser(web_socket* ws)
{
	try
	{
		UserData* data = ws->getUserData();

		if (Database::getInstance()->CheckEmailExists(data->get_email()))
		{
			data->set_id(Database::getInstance()->GetUserIdByEmail(data->get_email()));
			std::cout << "User connected ID: " << data->get_id() << "\n";
		}

		else
		{
			std::string errorMessage = "User with email " + data->get_email() + " not found";
			ws->send(errorMessage);
		}

		// connect the person to a private channel (for personal messages)
		ws->subscribe("userN" + std::to_string(data->get_id()));
		// connect to a common channel (for general chat)
		ws->subscribe("public_chat");

	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	
}

std::string MessagerHandler::ProcessUserStatus(UserData* data, bool online)
{
	json response;
	response[COMMAND] = STATUS;
	response[NAME] = data->get_name();
	response[RECEIVER_ID] = data->get_id();
	response[ONLINE] = online;
	return response.dump();
}

void MessagerHandler::ProcessSetName(web_socket* WS, json parsed, UserData* data)
{
	data->set_name(std::move(parsed[NAME]));
}

void MessagerHandler::ProcessPrivateMessage(web_socket* WS, json parsed, std::uint64_t user_id)
{
	UserData* data = WS->getUserData();
	const uint64_t user_id_to = parsed[RECEIVER_ID];
	std::string user_msg = parsed[MESSAGE];
	json response;

	response[COMMAND] = PRIVATE_MSG;
	response[MESSAGE] = user_msg;
	response[USER_ID_FROM] = data->get_id();

	WS->publish("userN" + std::to_string(user_id_to), response.dump()); // sending a message
}

void MessagerHandler::ProcessPublicMessage(web_socket* WS, json parsed, std::uint64_t user_id)
{
	UserData* data = WS->getUserData();
	std::string user_msg = parsed[MESSAGE];

	json response; // create a response for the general chat

	response[COMMAND] = PUBLIC_MSG;
	response[MESSAGE] = user_msg;
	response[USER_ID_FROM] = data->get_id();

	WS->publish("public_chat", response.dump()); // sending a message
}

void MessagerHandler::ProcessMessage(web_socket* WS, std::string_view message)
{
	UserData* data = WS->getUserData();

	std::cout << "Message from user ID: " << data->get_id() << "--message: " << message << '\n';
	auto parsed = json::parse(message);

	if (parsed[COMMAND] == PRIVATE_MSG) {
		ProcessPrivateMessage(WS, parsed, data->get_id());
	}

	if (parsed[COMMAND] == PUBLIC_MSG) {
		ProcessPublicMessage(WS, parsed, data->get_id());
	}

	if (parsed[COMMAND] == SET_NAME) {
		ProcessSetName(WS, parsed, data);
	}
}

void MessagerHandler::DisconnectedUser(web_socket* ws, int code, std::string_view message)
{
	std::cout << "User disconnected" << "\n";
}
