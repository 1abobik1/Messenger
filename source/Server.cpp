#define _CRT_SECURE_NO_WARNINGS

#include "../header/Server.h"
#include "../header/CommonConst.h"

#include <iostream>

using namespace JsonChat;

void Server::HandleSignUp(uWS::HttpResponse<true>* res, uWS::HttpRequest* req)
{
	// CORS policy
	res->writeHeader("Access-Control-Allow-Origin", "*");

	auto isAborted = std::make_shared<bool>(false);
	auto body = std::make_shared<std::string>();

	res->onData([res, isAborted, body](std::string_view chunk, bool isFin) mutable {
		if (!chunk.empty()) {
			// fill in the request body
			body->append(chunk.data(), chunk.length());

			if (isFin && !*isAborted) {
				// Parse JSON from the body
				json userData = json::parse(std::move(*body));

				// Retrieving data from a request
				const std::string email = std::move(userData["email"]);
				const std::string password = std::move(userData["pswd"]);
				const std::string user_name = std::move(userData["user_name"]);

				std::cout << "Received data in signup: " << user_name << ' ' << email << ' ' << password << '\n';

				try {
					// save data in the database
					Database::getInstance()->insert_user(user_name, email, password);  
					res->end("Signup successful!");
				}
				catch (std::exception& e) {
					//  errors messages
					std::cerr << e.what();
					res->writeStatus("500 Internal Server Error");
					res->end("Signup failed: " + static_cast<std::string>(e.what()));
				}

				// send a message that the server has received the JSON data
				res->end("The server received the signup-data");
			}
		}
		});

	res->onAborted([&isAborted]() {
		*isAborted = true;
		});
}

void Server::HandleLogIn(uWS::HttpResponse<true>* res, uWS::HttpRequest* req)
{
	// CORS policy
	res->writeHeader("Access-Control-Allow-Origin", "*");

	auto isAborted = std::make_shared<bool>(false);
	auto body = std::make_shared<std::string>();

	res->onData([res, isAborted, body](std::string_view chunk, bool isFin) mutable {
		if (!chunk.empty()) {
			// fill in the request body
			body->append(chunk.data(), chunk.length());

			if (isFin && !*isAborted) {
				// Parse JSON from the body
				json userData = json::parse(std::move(*body));

				// Retrieving data from a request
				const std::string email = std::move(userData["email"]);
				const std::string password = std::move(userData["pswd"]);

				std::cout << "Received data in login: "<< ' ' << email << ' ' << password << '\n';

				// send a message that the server has received the JSON data
				res->end("The server received the login-data");
			}
		}
		});

	res->onAborted([&isAborted]() {
		*isAborted = true;
		});
}

void Server::ProcessSetName(web_socket* WS, json parsed, UserData* data)
{
	data->name = parsed[NEW_NAME];
}

void Server::ProcessPrivateMessage(web_socket* WS, json parsed, std::uint64_t user_id)
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

void Server::ProcessPublicMessage(web_socket* WS, json parsed, std::uint64_t user_id)
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
	uWS::SSLApp({

		.key_file_name = "C:/Users/dima1/source/repos/ChatServer/misc/key.pem",
		.cert_file_name = "C:/Users/dima1/source/repos/ChatServer/misc/cert.pem",
		.passphrase = "dima15042004"

		}).post("/signup", [&](auto* res, auto* req) {
			HandleSignUp(res, req);

		}).post("/login", [&](auto* res, auto* req){
			HandleLogIn(res, req);
		})

		.ws<UserData>("/*",{

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
