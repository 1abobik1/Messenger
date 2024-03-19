#define _CRT_SECURE_NO_WARNINGS					
#define _SILENCE_ALL_CXX23_DEPRECATION_WARNINGS 

#include <uwebsockets/App.h>
#include <iostream>
#include <string_view>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

const std::string COMMAND = "command";

const std::string MESSAGE = "message";
const std::string PRIVATE_MSG = "private_msg";
const std::string PUBLIC_MSG = "public_msg";

const std::string RECEIVER_ID = "receiver_id";
const std::string USER_ID_FROM = "user_id_from";

const std::string SET_NAME = "set_name";
const std::string NEW_NAME = "new_name";

// пользователь
struct UserData
{
	uint64_t user_id;
    std::string name = "NO_NAME";
};

int main() {
    using namespace std;

    //id пользователей
    uint64_t cnt_user = 1;

    //настройки сервера
    // /* - значит что запускаем в корнейвой папке, пока без url
    uWS::App().ws<UserData>("/*", {
        .idleTimeout = 960,
	
	    .open = [&cnt_user](auto* ws)
	    {
	        UserData* data = ws->getUserData();
	        data->user_id = cnt_user++;

	        cout << "New user connected ID: " << data->user_id << "\n";

	        // подключаем человека личному каналу(личные сообщения)
	        ws->subscribe("userN" + to_string(data->user_id));

			//подключаем на общий канал(общий чат)
			ws->subscribe("public + ' ' + chat");
	    },

	    .message = [](auto* ws, string_view message, uWS::OpCode)
	    {
	    	UserData* data = ws->getUserData();

	        cout << "message from user ID: " << data->user_id << "--message: " << message << "\n";

	        auto parsed = json::parse(message);

	        if (parsed[COMMAND] == PRIVATE_MSG)
	        {
		        const uint64_t user_id_to = parsed[RECEIVER_ID];
	            string user_msg = parsed[MESSAGE];

				json response; // ответ получателю

				if(data->name != "NO_NAME")
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
	            
	            ws->publish("userN" + to_string(user_id_to), response.dump()); //отправляем в приватный канал.
	        	//response.dump() отправляет пользователю сообщение
	        }

			if(parsed[COMMAND] == PUBLIC_MSG)
			{
				string user_msg = parsed[MESSAGE];

				json response; // ответ всем

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

				ws->publish("public + ' ' + chat", response.dump()); // отправляем в общий канал
			}

			if(parsed[COMMAND] == SET_NAME)
			{
				data->name = parsed[NEW_NAME];
			}
	    },

	    .close = [](auto* ws, int code, string_view message)
	    {
	        cout << "User disconnected ID: " << ws->getUserData()->user_id << "\n";
	    } 

    }).listen(9001, [](auto*)
    {
    	cout << "Listening for connections..." << "\n";

    }).run();

}
// connection = new WebSocket("ws://localhost:9001/"); connection.onmessage = ({data}) => console.log(data); подключение пользователя и вывод на консоль js

//connection.send(JSON.stringify({"command":"private_msg","receiver_id":1,"message":"hi,im 2"})); пример отправки сообщения от пользователя 2 пользователю 1
// что видит получатель(receiver_id) {"command":"private_msg","message":"hi,i 2","user_id_from":2}

//connection.send(JSON.stringify({"command":"set_name", "new_name": "huesos"})) установка имени и теперь будет показываться его имя, если он отправит новое сообщение