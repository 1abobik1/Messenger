#pragma once

#include <uwebsockets/WebSocket.h>
#include <nlohmann/json.hpp>

#include "UserData.h"
#include "DataBase.h"

using json = nlohmann::json;

class MessagerHandler
{
private:
    typedef uWS::WebSocket<true, true, UserData> web_socket;

    void ConnectedUser(web_socket* ws);

    std::string ProcessUserStatus(UserData* data, bool online);

	void ProcessSetName(web_socket* WS, json parsed, UserData* data);

	void ProcessPrivateMessage(web_socket* WS, json parsed, std::uint64_t user_id);

	void ProcessPublicMessage(web_socket* WS, json parsed, std::uint64_t user_id);

	void ProcessMessage(web_socket* WS, std::string_view message);

    void DisconnectedUser(web_socket* ws, int code, std::string_view message);
public:
    friend class Server;
};
