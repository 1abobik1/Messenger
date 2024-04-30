#define _CRT_SECURE_NO_WARNINGS

#include "../header/MessagerHandler.h"
#include <iostream>

#include "../header/Server.h"
#include "../header/CommonConst.h"

using namespace JsonChat;


void MessagerHandler::ConnectedUser(web_socket* WS) {
	//debugging code
	std::cout << '\n';
	std::cout << "ConnectedUser__Start.." << '\n' << '\n';

	WebSocketUser* web_socket_data = WS->getUserData();
	web_socket_data->email_ = request_handler_->getUserModel()->get_email();
	std::cout << "web_socket_data->email_: " << web_socket_data->email_ << '\n';

	/*web_socket_data->name_ = request_handler_->getUserModel()->get_name();
	std::cout << "web_socket_data->name_: " << web_socket_data->name_ << '\n';

	web_socket_data->password_ = request_handler_->getUserModel()->get_password();
	std::cout << "web_socket_data->password_: " << web_socket_data->password_ << '\n';

	web_socket_data->id_ = request_handler_->getUserModel()->get_id();
	std::cout << "web_socket_data->id_: " << web_socket_data->id_ << '\n' << '\n';*/

	if (Database::getInstance()->CheckEmailExists(web_socket_data->email_)) 
	{
		web_socket_data->id_ = Database::getInstance()->GetUserIdByEmail(web_socket_data->email_);
		std::cout << "User connected ID: " << web_socket_data->id_ << '\n';
	}
	else
	{
		std::string errorMessage = "User with email " + web_socket_data->email_ + " not found ";
		std::cout << errorMessage;
	}

	// Подключение к каналам
	WS->subscribe("userN" + std::to_string(web_socket_data->id_));
	WS->subscribe("public_chat");
	std::cout << '\n';
	std::cout << "ConnectedUser__END.." << '\n';
}


std::string MessagerHandler::ProcessUserStatus(WebSocketUser* web_socket_data, bool online)
{
	json response;
	response[COMMAND] = STATUS;
	response[NAME] = web_socket_data->name_;
	response[RECEIVER_ID] = web_socket_data->id_;
	response[ONLINE] = online;
	return response.dump();
}

void MessagerHandler::ProcessSetName(web_socket* WS, json parsed, WebSocketUser* data)
{
	data->name_ = parsed[NAME];
}

void MessagerHandler::ProcessPrivateMessage(web_socket* WS, json parsed, std::uint64_t user_id)
{
	WebSocketUser* web_socket_data = WS->getUserData();

	const uint64_t user_id_to = parsed[RECEIVER_ID];
	std::string user_msg = parsed[MESSAGE];
	json response;

	response[COMMAND] = PRIVATE_MSG;
	response[MESSAGE] = user_msg;
	response[USER_ID_FROM] = web_socket_data->id_;

	WS->publish("userN" + std::to_string(user_id_to), response.dump()); // sending a message
}

void MessagerHandler::ProcessPublicMessage(web_socket* WS, json parsed, std::uint64_t user_id)
{
	WebSocketUser* web_socket_data = WS->getUserData();

	std::string user_msg = parsed[MESSAGE];

	json response; // create a response for the general chat

	response[COMMAND] = PUBLIC_MSG;
	response[MESSAGE] = user_msg;
	response[USER_ID_FROM] = web_socket_data->id_;

	WS->publish("public_chat", response.dump()); // sending a message
}

void MessagerHandler::ProcessMessage(web_socket* WS, std::string_view message)
{
	WebSocketUser* web_socket_data = WS->getUserData();

	std::cout << "Message from user ID: " << web_socket_data->id_ << "--message: " << message << '\n';
	auto parsed = json::parse(message);

	if (parsed[COMMAND] == PRIVATE_MSG) {
		ProcessPrivateMessage(WS, parsed, web_socket_data->id_);
	}

	if (parsed[COMMAND] == PUBLIC_MSG) {
		ProcessPublicMessage(WS, parsed, web_socket_data->id_);
	}

	if (parsed[COMMAND] == SET_NAME) {
		ProcessSetName(WS, parsed, web_socket_data);
	}
}

void MessagerHandler::DisconnectedUser(web_socket* WS, int code, std::string_view message)
{
	WebSocketUser* web_socket_data = WS->getUserData();

	std::cout << "User disconnected:  " << web_socket_data->id_ <<  '\n';
}

