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

	res->onData([res, isAborted, body, this](std::string_view chunk, bool isFin) mutable {
		if (!chunk.empty()) {
			// fill in the request body
			body->append(chunk.data(), chunk.length());

			if (isFin && !*isAborted) {
				// Parse JSON from the body
				json user_data_json = json::parse(std::move(*body));

				// Retrieving data from a request
				UserData_->set_email(std::move(user_data_json["email"]));
				UserData_->set_password(std::move(user_data_json["pswd"]));
				UserData_->set_name(std::move(user_data_json["user_name"]));

				std::cout << "Received data in signup: " << UserData_->get_name() << ' ' << UserData_->get_email() << ' ' << UserData_->get_password() << '\n';

				try {
					if (Database::getInstance()->CheckEmailExists(UserData_->get_email()))
					{
						throw std::exception("User with this email already exists");
					}
					else
					{
						// save data in database
						Database::getInstance()->InsertUsers(UserData_->get_name(), UserData_->get_email(), UserData_->get_password());
						res->writeStatus("200 OK");
						res->end("Signup successful!");
						std::cout << "Signup successful!" << '\n';
					}
				}
				catch (std::runtime_error& r) {
					//  errors messages
					std::cerr << r.what() << '\n';
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

	res->onData([res, isAborted, body, this](std::string_view chunk, bool isFin) mutable {
		if (!chunk.empty()) {
			// fill in the request body
			body->append(chunk.data(), chunk.length());

			if (isFin && !*isAborted) {

				// Parse JSON from the body
				json user_data_json = json::parse(std::move(*body));

				// Retrieving data from a request
				UserData_->set_email(std::move(user_data_json["email"]));
				UserData_->set_password(std::move(user_data_json["pswd"]));

				std::cout << "Received data in login:  " << Database::getInstance()->GetUserIdByEmail(UserData_->get_email()) << ' ' << UserData_->get_email() << ' ' << UserData_->get_password() << '\n';
				try {
					if (Database::getInstance()->CheckEmailExists(UserData_->get_email()))
					{
						if (bcrypt::validatePassword(UserData_->get_password(), /*hash-*/Database::getInstance()->GetPasswordByEmail(UserData_->get_email())))
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
				catch (std::runtime_error& r) {
					//  errors messages
					std::cerr << r.what() << '\n';
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

void RequestHandler::HandledDisplayUsers(uWS::HttpResponse<true>* res, uWS::HttpRequest* req)
{
	res->writeHeader("Access-Control-Allow-Origin", "*");

	auto isAborted = std::make_shared<bool>(false);

	std::cout << "HandledDisplayUsers working.." << '\n';

	res->onData([res, isAborted](std::string_view chunk, bool isFin) mutable {
		if (!chunk.empty()) {
			if (isFin && !*isAborted) {

				const json display_users = Database::getInstance()->GetAllUsersNamesInJson();
				res->end(display_users.dump()); //conversion to string 
				std::cout << "send: " << display_users.dump();
			}
		}
		});

	res->onAborted([&isAborted]() {
		*isAborted = true;
		});
}

void RequestHandler::HandleSearchUser(uWS::HttpResponse<true>* res, uWS::HttpRequest* req)
{
	res->writeHeader("Access-Control-Allow-Origin", "*");

	auto isAborted = std::make_shared<bool>(false);
	auto body = std::make_shared<std::string>();

	res->onData([res, isAborted, body, this](std::string_view chunk, bool isFin) mutable {
		if (!chunk.empty()) {
			body->append(chunk.data(), chunk.length());

			if (isFin && !*isAborted) {
				json json_user_email = json::parse(std::move(*body));
				const std::string email = std::move(json_user_email["email"]);

				std::cout << "searching email.. " << email << '\n';

				if (Database::getInstance()->CheckEmailExists(email)) {
					auto user_name = Database::getInstance()->FindUserByEmail(email);
					res->end(json({ {"username", user_name} }).dump());
					std::cout << "send: " << json({ {"username", user_name} }).dump() << '\n';
				}
				else {
					res->end(json({ {"error", "User not found"} }).dump());
				}
			}
		}
		});

	res->onAborted([&isAborted]() {
		*isAborted = true;
		});
}


