#define _CRT_SECURE_NO_WARNINGS

#include "../header/RequestHandler.h"
#include "../header/Server.h"
#include "../bcrypt-cpp/include/bcrypt.h"
#include "../header/DataBase.h"

#include <iostream>


void RequestHandler::HandleSignUp(uWS::HttpResponse<true>* res, uWS::HttpRequest* req)
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
				const auto email = std::make_unique<std::string>(std::move(userData["email"]));
				const auto password = std::make_unique<std::string>(std::move(userData["pswd"]));
				const auto user_name = std::make_unique<std::string>(std::move(userData["user_name"]));

				std::cout << "Received data in signup: " << *user_name << ' ' << *email << ' ' << *password << '\n';

				try {
					if (Database::getInstance()->CheckEmailExists(*email))
					{
						throw std::exception("User with this email already exists");
					}
					else
					{
						// save data in database
						Database::getInstance()->InsertUsers(*user_name, *email, *password);
						res->writeStatus("200 OK");
						res->end("Signup successful!");
						std::cout << "Signup successful!" << '\n';
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
				res->end("The server received the signup-data");
			}
		}
		});

	res->onAborted([&isAborted]() {
		*isAborted = true;
		});
}

void RequestHandler::HandleLogIn(uWS::HttpResponse<true>* res, uWS::HttpRequest* req)
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
				const auto email = std::make_unique<std::string>(std::move(userData["email"]));
				const auto password = std::make_unique<std::string>(std::move(userData["pswd"]));

				std::cout << "Received data in login: " << ' ' << *email << ' ' << *password << '\n';
				try {
					if (Database::getInstance()->CheckEmailExists(*email))
					{
						if (bcrypt::validatePassword(*password, /*hash-*/Database::getInstance()->GetPasswordByEmail(*email)))
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

