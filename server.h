#pragma once

#include <nlohmann/json.hpp>
#include <uwebsockets/App.h>
#include <iostream>
#include "CommonConst.h"

using json = nlohmann::json;

// §á§à§Ý§î§Ù§à§Ó§Ñ§ä§Ö§Ý§î
struct UserData
{
	uint64_t user_id;
	std::string name = "NO_NAME";
};

//id §á§à§Ý§î§Ù§à§Ó§Ñ§ä§Ö§Ý§ñ
inline uint32_t cnt_user = 1;

inline void runServer(const uint16_t port)
{
	using namespace JsonObj; using namespace PathHTML;

	// /* - §Ù§ß§Ñ§é§Ú§ä §é§ä§à §Ù§Ñ§á§å§ã§Ü§Ñ§Ö§Þ §Ó §Ü§à§â§ß§Ö§Ó§à§Û §á§Ñ§á§Ü§Ö, §á§à§Ü§Ñ §Ò§Ö§Ù url
	uWS::App().ws<UserData>("/*", {
			.idleTimeout = 960,

			.open = [](auto* ws)
			{
				UserData* data = ws->getUserData();
				data->user_id = cnt_user++;

				std::cout << "New user connected ID: " << data->user_id << "\n";

				// §á§à§Õ§Ü§Ý§ð§é§Ñ§Ö§Þ §é§Ö§Ý§à§Ó§Ö§Ü§Ñ §Ü §Ý§Ú§é§ß§à§Þ§å §Ü§Ñ§ß§Ñ§Ý§å(§Õ§Ý§ñ §Ý§Ú§é§ß§í§ç §ã§à§à§Ò§ë§Ö§ß§Ú§Û)
				ws->subscribe("userN" + std::to_string(data->user_id));

				// §á§à§Õ§Ü§Ý§ð§é§Ñ§Ö§Þ §Ü §à§Ò§ë§Ö§Þ§å §Ü§Ñ§ß§Ñ§Ý§å(§Õ§Ý§ñ §à§Ò§ë§Ö§Ô§à §é§Ñ§ä§Ñ)
				ws->subscribe("public + ' ' + chat");
			},

			.message = [](auto* ws, std::string_view message, uWS::OpCode)
			{
				UserData* data = ws->getUserData();

				std::cout << "message from user ID: " << data->user_id << "--message: " << message << "\n";

				auto parsed = json::parse(message);

				if (parsed[COMMAND] == PRIVATE_MSG)
				{
					const uint64_t user_id_to = parsed[RECEIVER_ID];
					std::string user_msg = parsed[MESSAGE];

					json response; // §ã§à§Ù§Õ§Ñ§Ö§Þ §à§ä§Ó§Ö§ä §á§à§Ý§å§é§Ñ§ä§Ö§Ý§ð

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

					ws->publish("userN" + std::to_string(user_id_to), response.dump()); //îòïðàâ?åì ?ïðèâàòíû?êàíà?
					//response.dump() §à§ä§á§â§Ñ§Ó§Ý§ñ§Ö§ä §á§à§Ý§î§Ù§à§Ó§Ñ§ä§Ö§Ý§ð §ã§à§à§Ò§ë§Ö§ß§Ú§Ö
				}

				if (parsed[COMMAND] == PUBLIC_MSG)
				{
					std::string user_msg = parsed[MESSAGE];

					json response; // §ã§à§Ù§Õ§Ñ§Ö§Þ §à§ä§Ó§Ö§ä §Õ§Ý§ñ §à§Ò§ë§Ö§Ô§à §é§Ñ§ä§Ñ

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

					ws->publish("public + ' ' + chat", response.dump()); // îòïðàâ?åì ?îáùè?êàíà?
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

		}).listen(port, [](auto*)
			{
				std::cout << "Listening for connections..." << "\n";

			}).run();
}