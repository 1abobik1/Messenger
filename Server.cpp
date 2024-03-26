#define _CRT_SECURE_NO_WARNINGS
#include <uwebsockets/App.h>
#include <nlohmann/json.hpp>

#include <iostream>
#include<vector>
#include<thread>
#include<algorithm>

#include "CommonConst.h"
#include "Server.h"

using json = nlohmann::json;

using namespace JsonObj;
using namespace PathHTML;

void Server::run()
{

	std::vector<std::thread*> threads(std::thread::hardware_concurrency());
	std::ranges::transform(threads, threads.begin(), [&](auto* thr)
		{
			return new std::thread([&]()
				{
					// /* - значит что запускаем в корневой папке, пока без url
					uWS::SSLApp({

					 .key_file_name = "misc/key.pem",
					 .cert_file_name = "misc/cert.pem",
					 .passphrase = "Zopa_kek12"

					}).ws<UserData>("/*", {
						.idleTimeout = 960,

						.open = [&](auto* ws)
						{
						  UserData* data = ws->getUserData();
						  data->user_id = this->cnt_user_++;

						  std::cout << "New user connected ID: " << data->user_id << "\n";

						  // подключаем человека к личному каналу(дл€ личных сообщений)
						  ws->subscribe("userN" + std::to_string(data->user_id));

						  // подключаем к общему каналу(дл€ общего чата)
						  ws->subscribe("public_chat");
						},

						.message = [](auto* ws, std::string_view message, uWS::OpCode)
						{
						  UserData* data = ws->getUserData();

						  std::cout << "message from user ID: " << data->user_id << " --message: " << message << "\n";

						  auto parsed = json::parse(message);

						  if (parsed[COMMAND] == PRIVATE_MSG)
						  {
							const uint64_t user_id_to = parsed[RECEIVER_ID];
							std::string user_msg = parsed[MESSAGE];

							json response; // создаем ответ получателю

							if (data->name != "NO_NAME")
							{
							  response[COMMAND] = PRIVATE_MSG;
							  response[MESSAGE] = user_msg;
							  response[USER_ID_FROM] = data->name;
							}

							else
							{
							  response[COMMAND] = PRIVATE_MSG;
							  response[MESSAGE] = user_msg;
							  response[USER_ID_FROM] = data->user_id;

							}

							ws->publish("userN" + std::to_string(user_id_to), response.dump()); // отправка сообщени€
							//response.dump() отправл€ет пользователю сообщение
						  }

						  if (parsed[COMMAND] == PUBLIC_MSG)
						  {
							std::string user_msg = parsed[MESSAGE];

							json response; // создаем ответ дл€ общего чата

							if (data->name != "NO_NAME")
							{
							  response[COMMAND] = PUBLIC_MSG;
							  response[MESSAGE] = user_msg;
							  response[USER_ID_FROM] = data->name;
							}

							else
							{
							  response[COMMAND] = PUBLIC_MSG;
							  response[MESSAGE] = user_msg;
							  response[USER_ID_FROM] = data->user_id;
							}

							ws->publish("public_chat", response.dump()); // отправка сообщени€
						  }

						  if (parsed[COMMAND] == SET_NAME)
						  {
							data->name = parsed[NEW_NAME];
						  }
						},

						.close = [](auto* ws, int code, std::string_view message)
						{
						  std::cout << "User disconnected ID: " << ws->getUserData()->user_id << "\n";
						}

						}).listen(this->port_, [](const auto* listenSocket)
							{
								if (listenSocket) {
									//std::cout << "Listening on port 9001 thread: "<< std::this_thread::get_id() << '\n';
								}
								else {
									std::cout << "Failed to load certificates or bind to port." << '\n';
								}
							}).run();
				});
		});

	for (const auto thread : threads)
	{
		thread->join();
	}
}
