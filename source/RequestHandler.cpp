#define _CRT_SECURE_NO_WARNINGS

#include "../header/RequestHandler.h"
#include "../header/Server.h"
#include "../bcrypt-cpp/include/bcrypt.h"
#include "../header/DataBase.h"

#include <iostream>


void RequestHandler::HandleSignUp(uWS::HttpResponse<false>* res, uWS::HttpRequest* req)
{
	std::cout << '\n';

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
				json client_json = json::parse(std::move(*body));

				// Retrieving data from a request
				user_model_->set_email(std::move(client_json["email"]));
				user_model_->set_password(std::move(client_json["pswd"]));
				user_model_->set_name(std::move(client_json["user_name"]));

				std::cout << "Received data in signup: " << user_model_->get_name() << ' ' << user_model_->get_email() << ' ' << user_model_->get_password() << '\n';

				try {
					if (Database::getSingleItem()->CheckEmailExists(user_model_->get_email()))
					{
						throw std::exception("User with this email already exists");
					}
					else
					{
						// save data in database
						Database::getSingleItem()->InsertUsers(user_model_->get_name(), user_model_->get_email(), user_model_->get_password());
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

void RequestHandler::HandleLogIn(uWS::HttpResponse<false>* res, uWS::HttpRequest* req)
{
	std::cout << '\n';

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
				json client_json = json::parse(std::move(*body));

				// Retrieving data from a request
				user_model_->set_email(std::move(client_json["email"]));
				user_model_->set_password(std::move(client_json["pswd"]));

				std::cout << "Received data in login:  " << ' ' << user_model_->get_email() << ' ' << user_model_->get_password() << '\n';
				try {
					if (Database::getSingleItem()->CheckEmailExists(user_model_->get_email()))
					{
						if (bcrypt::validatePassword(user_model_->get_password(), /*hash-*/Database::getSingleItem()->GetPasswordByEmail(user_model_->get_email())))
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

void RequestHandler::HandlePrintClientsMessages(uWS::HttpResponse<false>* res,uWS::HttpRequest* req)
{
	std::cout << '\n';
	res->writeHeader("Access-Control-Allow-Origin", "*");
	std::cout << "HandleClientsMessages working.." << '\n';

	auto isAborted = std::make_shared<bool>(false);
	auto body = std::make_shared<std::string>();

	res->onData([res, isAborted, body](std::string_view chunk, bool isFin) mutable {
		if (!chunk.empty()) {
			body->append(chunk.data(), chunk.length());

			if (isFin && !*isAborted) {
				// Parse JSON from the body
				json client_json = json::parse(std::move(*body));
				json array_users_msg = json::array();

				const uint64_t sender_id = std::move(client_json["sender_id"]);
				const uint64_t receiver_id = std::move(client_json["receiver_id"]);

				array_users_msg = Database::getSingleItem()->PrintClientsMessages(sender_id, receiver_id);

				res->end(array_users_msg.dump());
			}
		}
		});

	res->onAborted([&isAborted]() {
		*isAborted = true;
		});
}

void RequestHandler::HandleSearchUser(uWS::HttpResponse<false>* res, uWS::HttpRequest* req)
{
	std::cout << '\n';

	res->writeHeader("Access-Control-Allow-Origin", "*");

	auto isAborted = std::make_shared<bool>(false);
	auto body = std::make_shared<std::string>();

	res->onData([res, isAborted, body, this](std::string_view chunk, bool isFin) mutable {
		if (!chunk.empty()) {
			body->append(chunk.data(), chunk.length());

			if (isFin && !*isAborted) {
				json client_json = json::parse(std::move(*body));
				const std::string query = std::move(client_json["query"]);

				std::cout << "Searching user.. " << query << '\n';

				auto user = Database::getSingleItem()->FindUserByEmail(query);
				if (!user.empty()) {
					uint64_t user_id = Database::getSingleItem()->GetUserIdByEmail(query);
					
					res->end(json({ {"user_by_email", user}, {"user_id", user_id} }).dump());

					std::cout << json({ {"user_by_email", user}, {"user_id", user_id} }) << '\n';
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

void RequestHandler::HandleSearchUserFriends(uWS::HttpResponse<false>* res, uWS::HttpRequest* req)
{
	std::cout << '\n';
	res->writeHeader("Access-Control-Allow-Origin", "*");
	std::cout << "HandleSearchUsersFriends working.." << '\n';

	auto isAborted = std::make_shared<bool>(false);
	auto body = std::make_shared<std::string>();

	res->onData([res, isAborted, body](std::string_view chunk, bool isFin) mutable {
		if (!chunk.empty()) {
			body->append(chunk.data(), chunk.length());

			if (isFin && !*isAborted) {
				// Parse JSON from the body
				json client_json = json::parse(std::move(*body));
				json users_friends = json::array();

				std::cout<< "client_json_email " << client_json["user_email"] << '\n';
				const std::string user_email = std::move(client_json["user_email"]);

				const uint64_t user_id = Database::getSingleItem()->GetUserIdByEmail(user_email);
				users_friends = Database::getSingleItem()->GetFriendIdsByUserId(user_id);

				std::cout << users_friends.dump();
				res->end(users_friends.dump());
			}
		}
	});

	res->onAborted([&isAborted]() {
		*isAborted = true;
		});
}

void RequestHandler::HandleAddFriend(uWS::HttpResponse<false>* res, uWS::HttpRequest* req)
{
	std::cout << '\n';
	res->writeHeader("Access-Control-Allow-Origin", "*");
	std::cout << "HandleAddFriend working.." << '\n';

	auto isAborted = std::make_shared<bool>(false);
	auto body = std::make_shared<std::string>();

	res->onData([res, isAborted, body](std::string_view chunk, bool isFin) mutable {
		if (!chunk.empty()) {
			body->append(chunk.data(), chunk.length());

			if (isFin && !*isAborted) {
				// Parse JSON from the body
				json client_json = json::parse(std::move(*body));
				const std::string user_email = std::move(client_json["user_email"]);
				const uint64_t new_friend_id = std::move(client_json["new_friend_id"]);
				std::cout << "user_email  " << user_email << " new_friend_id  " << new_friend_id<< '\n';

				const uint64_t user_id = Database::getSingleItem()->GetUserIdByEmail(user_email);
				Database::getSingleItem()->AddFriendForUser(user_id, new_friend_id);

				res->end("200");
			}
		}
	});

	res->onAborted([&isAborted]() {
		*isAborted = true;
		});
}


UserModel* RequestHandler::getUserModel() const
{
	return user_model_.get();
}
