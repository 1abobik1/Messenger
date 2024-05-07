#define _CRT_SECURE_NO_WARNINGS

#include "../header/MessagerHandler.h"
#include <iostream>

#include "../header/Server.h"
#include "../header/CommonConst.h"

using namespace JsonChat;

std::string time_utils::ExtractTime(const std::string& sent_at)
{
	if (const size_t space_pos = sent_at.find(' '); space_pos != std::string::npos && space_pos + 1 < sent_at.size()) {
		return sent_at.substr(space_pos + 1, 5);
	}

	return "";
}

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

	WS->subscribe("userN");
	std::cout << '\n';
	std::cout << "ConnectedUser__END.." << '\n';
}


void MessagerHandler::ProcessPrivateMessage( web_socket* WS, json parsed)
{
	std::cout << "start ProcessPrivateMessage.." << '\n';

	WebSocketUser* web_socket_data = WS->getUserData();

	web_socket_data->receiver_id_ = parsed[RECEIVER_ID];
	const std::string user_msg = parsed[MESSAGE];

	const std::string sent_at = Database::getInstance()->InsertAndGetSentAt(web_socket_data->sender_id_, web_socket_data->receiver_id_, user_msg);
	const std::string time_only = time_utils::ExtractTime(sent_at);

	std::cout << "time_only  " << time_only << '\n';

	json response;
	response[COMMAND] = PRIVATE_MSG;
	response[SENDER_ID] = web_socket_data->receiver_id_;
	response[MESSAGE] = user_msg;
	response[SENT_AT] = time_only;

	WS->publish("userN", response.dump()); // sending a message

	std::cout << "end ProcessPrivateMessage.." << '\n';
}

void MessagerHandler::ProcessMessage(web_socket* WS, std::string_view message)
{
	WebSocketUser* web_socket_data = WS->getUserData();

	std::cout << "Message from user ID: " << web_socket_data->sender_id_ << "--message: " << message << '\n';
	auto parsed = json::parse(message);

	if (parsed[COMMAND] == PRIVATE_MSG) {
		ProcessPrivateMessage(WS, parsed);
	}
}

void MessagerHandler::DisconnectedUser(web_socket* WS, int code, std::string_view message)
{
	WebSocketUser* web_socket_data = WS->getUserData();

	std::cout << "User disconnected:  " << web_socket_data->sender_id_ <<  '\n';
}

