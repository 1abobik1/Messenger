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
	web_socket_data->email_sender_id_ = request_handler_->getUserModel()->get_email();
	std::cout << "web_socket_data->email_: " << web_socket_data->email_sender_id_ << '\n';

	if (Database::getInstance()->CheckEmailExists(web_socket_data->email_sender_id_))
	{
		web_socket_data->sender_id_ = Database::getInstance()->GetUserIdByEmail(web_socket_data->email_sender_id_);
		std::cout << "User connected ID: " << web_socket_data->sender_id_ << '\n';
	}
	else
	{
		std::string errorMessage = "User with email " + web_socket_data->email_sender_id_ + " not found ";
		std::cout << errorMessage;
	}

	WS->subscribe("userN" + std::to_string(web_socket_data->sender_id_));// sending a message
	std::cout << '\n';
	std::cout << "ConnectedUser__END.." << '\n';
}


std::string MessagerHandler::ProcessUserStatus(WebSocketUser* web_socket_data, bool online)
{
	json response;
	response[COMMAND] = STATUS;
	response[RECEIVER_ID] = web_socket_data->sender_id_;
	response[ONLINE] = online;
	return response.dump();
}

void MessagerHandler::ProcessPrivateMessage(web_socket* WS, json parsed)
{
	//WebSocketUser* web_socket_data = WS->getUserData();

	//const uint64_t received_id = parsed[RECEIVER_ID]; // здесь будет скорее всего метод, в котором пользователь получает receiver_id(то есть id к кому хочет написать), 
	//std::string user_msg = parsed[MESSAGE];
	//json response;

	//response[COMMAND] = PRIVATE_MSG;
	//response[MESSAGE] = user_msg;
	//response[SENDER_ID] = web_socket_data->sender_id_;

	//// sending a message
	//std::string response_str = response.dump();
	//WS->publish("userN" + std::to_string(received_id), response_str);

	//// Отправка подтверждения отправки сообщения отправителю
	//response["status"] = "sent";
	//WS->publish("userN" + std::to_string(web_socket_data->sender_id_), response.dump());

	WebSocketUser* web_socket_data = WS->getUserData();

	const uint64_t received_id = parsed[RECEIVER_ID];
	std::string user_msg = parsed[MESSAGE];
	json response;

	response[COMMAND] = PRIVATE_MSG;
	response[MESSAGE] = user_msg;
	response[SENDER_ID] = web_socket_data->sender_id_;

	WS->publish("userN" + std::to_string(received_id), response.dump()); // sending a message
}

void MessagerHandler::ProcessPublicMessage(web_socket* WS, json parsed)
{
	WebSocketUser* web_socket_data = WS->getUserData();

	std::string user_msg = parsed[MESSAGE];

	json response; 

	response[COMMAND] = PUBLIC_MSG;
	response[MESSAGE] = user_msg;
	response[SENDER_ID] = web_socket_data->sender_id_;

	WS->publish("public_chat", response.dump()); // sending a message
}

void MessagerHandler::ProcessMessage(web_socket* WS, std::string_view message)
{
	WebSocketUser* web_socket_data = WS->getUserData();

	std::cout << "Message from user ID: " << web_socket_data->sender_id_ << "--message: " << message << '\n';
	auto parsed = json::parse(message);

	if (parsed[COMMAND] == PRIVATE_MSG) {
		ProcessPrivateMessage(WS, parsed);
	}

	if (parsed[COMMAND] == PUBLIC_MSG) {
		ProcessPublicMessage(WS, parsed);
	}
}

void MessagerHandler::DisconnectedUser(web_socket* WS, int code, std::string_view message)
{
	WebSocketUser* web_socket_data = WS->getUserData();

	std::cout << "User disconnected:  " << web_socket_data->sender_id_ <<  '\n';
}

