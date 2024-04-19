#pragma once
#include <uwebsockets/WebSocket.h>

#include <nlohmann/json.hpp>
#include "../header/CommonConst.h"

using json = nlohmann::json;

class MessagerHandler
{
protected:
	std::uint64_t cnt_UserData_ = 1;

    struct UserData
    {
        uint64_t user_id;
        std::string name = "NO_NAME";
    };

    typedef uWS::WebSocket<true, true, UserData> web_socket;

    static void ProcessSetName(web_socket* WS, json parsed, UserData* data);

    static void ProcessPrivateMessage(web_socket* WS, json parsed, std::uint64_t user_id);

    static void ProcessPublicMessage(web_socket* WS, json parsed, std::uint64_t user_id);

    static void ProcessMessage(web_socket* WS, std::string_view message);

    void ConnectedUser(web_socket* ws);

    void DisconnectedUser(web_socket* ws, int code, std::string_view message);
};
