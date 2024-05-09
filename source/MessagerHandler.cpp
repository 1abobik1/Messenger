#define _CRT_SECURE_NO_WARNINGS

#include "../header/MessagerHandler.h"
#include <iostream>

#include "../header/Server.h"
#include "../header/CommonConst.h"

using namespace JsonChat;

void MessagerHandler::ConnectedUser(web_socket* WS) {
	//debugging code
	std::cout << '\n';
	std::cout << "ConnectedUser__Start.." << '\n';

	WebSocketUser* web_socket_data = WS->getUserData();
	web_socket_data->email_sender_id_ = request_handler_->getUserModel()->get_email();
	std::cout << "web_socket_data->email_: " << web_socket_data->email_sender_id_ << '\n';

	if (Database::getSingleItem()->CheckEmailExists(web_socket_data->email_sender_id_))
	{
		web_socket_data->sender_id_ = Database::getSingleItem()->GetUserIdByEmail(web_socket_data->email_sender_id_);
		std::cout << "User connected ID: " << web_socket_data->sender_id_ << '\n';
	}
	else
	{
		std::string errorMessage = "User with email " + web_socket_data->email_sender_id_ + " not found ";
		std::cout << errorMessage;
	}

	WS->subscribe("userN" + std::to_string(web_socket_data->sender_id_));
	std::cout << "ConnectedUser__END.." << '\n';
}


void MessagerHandler::ProcessPrivateMessage( web_socket* WS, json parsed)
{
	std::cout << '\n';
	std::cout << "start ProcessPrivateMessage.." << '\n';

	WebSocketUser* web_socket_data = WS->getUserData();

	const std::string user_msg = parsed[MESSAGE];
	web_socket_data->receiver_id_ = parsed[RECEIVER_ID];

	const std::string sent_at = Database::getSingleItem()->InsertAndGetSentAt(web_socket_data->sender_id_, web_socket_data->receiver_id_, user_msg);

	std::cout << "time_only  " << sent_at << '\n';

	json response;
	response[COMMAND] = PRIVATE_MSG;
	response[SENDER_ID] = web_socket_data->receiver_id_;
	response[MESSAGE] = user_msg;
	response[SENT_AT] = sent_at;

	WS->publish("userN" + std::to_string(web_socket_data->receiver_id_), response.dump()); // sending a message

	std::cout << "end ProcessPrivateMessage.." << '\n';
}

void MessagerHandler::ProcessMessage(web_socket* WS, std::string_view message)
{
	std::cout << '\n';
	WebSocketUser* web_socket_data = WS->getUserData();

	std::cout << "Message from user ID: " << web_socket_data->sender_id_ << "--message: " << message << '\n';
	auto parsed = json::parse(message);

	if (parsed[COMMAND] == PRIVATE_MSG) {
		ProcessPrivateMessage(WS, parsed);
	}
}

void MessagerHandler::DisconnectedUser(web_socket* WS, int code, std::string_view message)
{
	std::cout << '\n';
	WebSocketUser* web_socket_data = WS->getUserData();

	std::cout << "User disconnected:  " << web_socket_data->sender_id_ <<  '\n';
}

