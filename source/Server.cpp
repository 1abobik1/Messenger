#define _CRT_SECURE_NO_WARNINGS

#include "../header/Server.h"
#include "../header/CommonConst.h"
#include "../bcrypt-cpp/include/bcrypt.h"

#include <iostream>
#include<fstream>
#include <future>

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
					if(Database::getInstance()->CheckEmailExists(email))
					{
						throw std::exception("User with this email already exists");
					}
					// save data in database
					Database::getInstance()->InsertUsers(user_name, email, password);
					res->writeStatus("200 OK");
					res->end("Signup successful!");
					std::cout << "Signup successful!" << '\n';
				}
				catch (std::runtime_error& e){
					//  errors messages
					std::cerr << e.what() << '\n';
					res->writeStatus("500 Internal Server Error");
					res->end("Internal Server Error");
				}
				catch (std::exception& e) {
					std::cerr << e.what() << '\n';
					res->writeStatus("409 Conflict");
					res->end(e.what());
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

				std::cout << "Received data in login: " << ' ' << email << ' ' << password << '\n';
				try {
					if (Database::getInstance()->CheckEmailExists(email))
					{
						if(bcrypt::validatePassword(password, /*hash-*/Database::getInstance()->GetPasswordByEmail(email)))
						{
							res->writeStatus("200 OK");
							res->end("LogIn successful!");
							std::cout << "LogIn successful!" << '\n';
						}
						else
						{
							throw std::exception("wrong password");
						}
					}
					else
					{
						throw std::exception("wrong email");
					}
				}
				catch (std::runtime_error& e) {
					//  errors messages
					std::cerr << e.what() << '\n';
					res->writeStatus("500 Internal Server Error");
					res->end("Internal Server Error");
				}
				catch (std::exception& e) {
					std::cerr << e.what() << '\n';
					res->writeStatus("409 Conflict");
					res->end(e.what());
				}
				// send a message that the server has received the JSON data
				res->end("The server received the login-data");
			}
		}
		});

	res->onAborted([&isAborted]() {
		*isAborted = true;
		});
}

void Server::RegPanelHTML(uWS::HttpResponse<true>* res, uWS::HttpRequest* req)
{
	res->writeHeader("Content-Type", "text/html");

	auto future_content = std::async(std::launch::async, [&]() {
		std::ifstream file_path("C:/Users/dima1/source/repos/ChatServer/web-files/index.html");
		if (file_path) {
			return std::string((std::istreambuf_iterator<char>(file_path)), std::istreambuf_iterator<char>());
		}
		return std::string("File not found");
		});

	res->end(future_content.get());
}

void Server::RegPanelCSS(uWS::HttpResponse<true>* res, uWS::HttpRequest* req)
{
	res->writeHeader("Content-Type", "text/css");

	auto future_content = std::async(std::launch::async, [&]() {
		std::ifstream file_path("C:/Users/dima1/source/repos/ChatServer/web-files/style.css");
		if (file_path) {
			return std::string((std::istreambuf_iterator<char>(file_path)), std::istreambuf_iterator<char>());
		}
		return std::string("File not found");
		});

	res->end(future_content.get());
}

void Server::RegPanelLoginJS(uWS::HttpResponse<true>* res, uWS::HttpRequest* req)
{
	res->writeHeader("Content-Type", "text/css");

	auto future_content = std::async(std::launch::async, [&]() {
		std::ifstream file_path("C:/Users/dima1/source/repos/ChatServer/web-files/login.js");
		if (file_path) {
			return std::string((std::istreambuf_iterator<char>(file_path)), std::istreambuf_iterator<char>());
		}
		return std::string("File not found");
		});

	res->end(future_content.get());
}

void Server::RegPanelSignupJS(uWS::HttpResponse<true>* res, uWS::HttpRequest* req)
{
	res->writeHeader("Content-Type", "text/css");

	auto future_content = std::async(std::launch::async, [&]() {
		std::ifstream file_path("C:/Users/dima1/source/repos/ChatServer/web-files/signup.js");
		if (file_path) {
			return std::string((std::istreambuf_iterator<char>(file_path)), std::istreambuf_iterator<char>());
		}
		return std::string("File not found");
		});

	res->end(future_content.get());
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
	ssl_app_
		.post("/signup", [&](auto* res, auto* req) {
			HandleSignUp(res, req);

		}).post("/login", [&](auto* res, auto* req){
			HandleLogIn(res, req);

		}).get("/", [this](auto* res, auto* req){
			RegPanelHTML(res, req);
		}).get("/style.css", [this](auto* res, auto* req){
			RegPanelCSS(res, req);
		}).get("/signup.js", [this](auto* res, auto* req) {
			RegPanelSignupJS(res, req);
		}).get("/login.js", [this](auto* res, auto* req) {
			RegPanelLoginJS(res, req);
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

		.message = [this](auto* ws, std::string_view message, uWS::OpCode)
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
				//std::cout << "Listening on port 9000 thread: "<< std::this_thread::get_id() << '\n';
			}
			else {
				std::cout << "Failed to load certificates or bind to port." << '\n';
			}
		}).run();
}
