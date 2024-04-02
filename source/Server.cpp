#define _CRT_SECURE_NO_WARNINGS

#include "../header/Server.h"
#include "../header/CommonConst.h"

#include <iostream>

using namespace JsonChat;

void Server::ProcessSetName(web_socket* WS, nlohmann::json parsed, UserData* data)
{
	data->name = parsed[NEW_NAME];
}

void Server::ProcessPrivateMessage(web_socket* WS, nlohmann::json parsed, std::uint64_t user_id)
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

void Server::ProcessPublicMessage(web_socket* WS, nlohmann::json parsed, std::uint64_t user_id)
{
	UserData* data = WS->getUserData();
	std::string user_msg = parsed[MESSAGE];
	json response; // create a response for the general chat

	response[COMMAND] = PUBLIC_MSG;
	response[MESSAGE] = user_msg;
	response[USER_ID_FROM] = data->user_id;

	WS->publish("public_chat", response.dump()); // sending a message

}

void Server::ProcessMessage(web_socket* WS, std::string_view message)
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

void Server::run()
{

	uWS::App()
		.get("/signup", [&](auto* res, auto* req)
		{
			auto body = static_cast<std::string>(req->getQuery());

			if(!body.empty())
			{
				json userData = json::parse(UrlDecode(body));

				// Получение данных из запроса
				const std::string email = userData["email"];
				const std::string password = userData["pswd"];
				const std::string user_name = userData["user_name"];

				std::cout << "Received data: " << user_name << ' ' << email << ' ' << password << '\n'; // Выводим полученные данные
			}
			res->writeStatus("200 OK");
			res->end("server http://localhost:9001/signup working");
		}).ws<UserData>("/*", {

		.idleTimeout = 666,

		.open = [&](auto* ws)
		{
			UserData* data = ws->getUserData();
		    data->user_id = this->cnt_user_++;

		    std::cout << "New user connected ID: " << data->user_id << "\n";
		    // connect the person to a private channel (for personal messages)
		    ws->subscribe("userN" + std::to_string(data->user_id));
		    // connect to a common channel (for general chat)
		    ws->subscribe("public_chat");
		},

		.message = [](auto* ws, std::string_view message, uWS::OpCode)
		{
			ProcessMessage(ws,message);
		},

		.close = [](auto* ws, int code, std::string_view message)
		{
			const UserData* data = ws->getUserData();
			std::cout << "User disconnected ID: " << data->user_id << "\n";
		}

		})
		.options("/*", [&](auto* res, auto* req) 
		{
			// for policy CORS and not only
			res->writeHeader("Access-Control-Allow-Origin", "*");
			res->writeHeader("Access-Control-Allow-Headers", "Content-Type");
			res->writeHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
			res->writeStatus("200 OK");
			res->end();

		})
		.listen(this->port_, [](const auto* listenSocket)
		{
			if (listenSocket) {
				//std::cout << "Listening on port 9001 thread: "<< std::this_thread::get_id() << '\n';
			}
			else {
				std::cout << "Failed to load certificates or bind to port." << '\n';
			}
		}).run();
}
