#define _CRT_SECURE_NO_WARNINGS

#include "../header/MessagerHandler.h"
#include <iostream>

#include "../header/Server.h"
#include "../header/CommonConst.h"

using namespace JsonChat;

void MessagerHandler::ProcessSetName(web_socket* WS, json parsed, UserData* data)
{
	data->name = parsed[NEW_NAME];
}

void MessagerHandler::ProcessPrivateMessage(web_socket* WS, json parsed, std::uint64_t user_id)
{
	UserData* data = WS->getUserData();
	const uint64_t user_id_to = parsed[RECEIVER_ID];
	std::string user_msg = parsed[MESSAGE];
	json response;

	response[COMMAND] = PRIVATE_MSG;
	response[MESSAGE] = user_msg;
	response[USER_ID_FROM] = data->user_id;

	WS->publish("userN" + std::to_string(user_id_to), response.dump()); // sending a message
	//response.dump() sends a message to the user
}

void MessagerHandler::ProcessPublicMessage(web_socket* WS, json parsed, std::uint64_t user_id)
{
	UserData* data = WS->getUserData();
	std::string user_msg = parsed[MESSAGE];
	json response; // create a response for the general chat

	response[COMMAND] = PUBLIC_MSG;
	response[MESSAGE] = user_msg;
	response[USER_ID_FROM] = data->user_id;

	WS->publish("public_chat", response.dump()); // sending a message
}

void MessagerHandler::ProcessMessage(web_socket* WS, std::string_view message)
{
	UserData* data = WS->getUserData();

	std::cout << "Message from user ID: " << data->user_id << "--message: " << message << '\n';
	auto parsed = json::parse(message);

	if (parsed[COMMAND] == PRIVATE_MSG) {
		ProcessPrivateMessage(WS, parsed, data->user_id);
	}

	if (parsed[COMMAND] == PUBLIC_MSG) {
		ProcessPublicMessage(WS, parsed, data->user_id);
	}

	if (parsed[COMMAND] == SET_NAME) {
		ProcessSetName(WS, parsed, data);
	}
}

void MessagerHandler::ConnectedUser(web_socket* ws)
{
	UserData* data = ws->getUserData();
	data->user_id = this->cnt_UserData_++;
	std::cout << "New user connected ID: " << data->user_id << "\n";
	// connect the person to a private channel (for personal messages)
	ws->subscribe("userN" + std::to_string(data->user_id));
	// connect to a common channel (for general chat)
	ws->subscribe("public_chat");
}

void MessagerHandler::DisconnectedUser(web_socket* ws, int code, std::string_view message)
{
	const UserData* data = ws->getUserData();
	std::cout << "User disconnected ID: " << data->user_id << "\n";
}
