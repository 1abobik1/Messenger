#pragma once

#include <nlohmann/json.hpp>
#include <uwebsockets/App.h>
#include <iostream>
#include "CommonConst.h"

using json = nlohmann::json;

// ���ݧ�٧�ӧѧ�֧ݧ�
struct UserData
{
	uint64_t user_id;
	std::string name = "NO_NAME";
};

//id ���ݧ�٧�ӧѧ�֧ݧ�
inline uint32_t cnt_user = 1;

inline void runServer(const uint16_t port)
{
	using namespace JsonObj; using namespace PathHTML;

	// /* - �٧ߧѧ�ڧ� ���� �٧ѧ���ܧѧ֧� �� �ܧ��ߧ֧ӧ�� ��ѧ�ܧ�, ���ܧ� �ҧ֧� url
	uWS::App().ws<UserData>("/*", {
			.idleTimeout = 960,

			.open = [](auto* ws)
			{
				UserData* data = ws->getUserData();
				data->user_id = cnt_user++;

				std::cout << "New user connected ID: " << data->user_id << "\n";

				// ���էܧݧ��ѧ֧� ��֧ݧ�ӧ֧ܧ� �� �ݧڧ�ߧ�ާ� �ܧѧߧѧݧ�(�էݧ� �ݧڧ�ߧ�� ����ҧ�֧ߧڧ�)
				ws->subscribe("userN" + std::to_string(data->user_id));

				// ���էܧݧ��ѧ֧� �� ��ҧ�֧ާ� �ܧѧߧѧݧ�(�էݧ� ��ҧ�֧ԧ� ��ѧ��)
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

					json response; // ���٧էѧ֧� ���ӧ֧� ���ݧ��ѧ�֧ݧ�

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

					ws->publish("userN" + std::to_string(user_id_to), response.dump()); //������?�� ?��������?����?
					//response.dump() �����ѧӧݧ�֧� ���ݧ�٧�ӧѧ�֧ݧ� ����ҧ�֧ߧڧ�
				}

				if (parsed[COMMAND] == PUBLIC_MSG)
				{
					std::string user_msg = parsed[MESSAGE];

					json response; // ���٧էѧ֧� ���ӧ֧� �էݧ� ��ҧ�֧ԧ� ��ѧ��

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

					ws->publish("public + ' ' + chat", response.dump()); // ������?�� ?����?����?
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