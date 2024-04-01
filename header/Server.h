#pragma once

#define _CRT_SECURE_NO_WARNINGS
#include <uwebsockets/App.h>
#include <nlohmann/json.hpp>

#include "CommonConst.h"

using json = nlohmann::json;

using namespace JsonObj;
using namespace PathHTML;


class Server
{
private:
    struct UserData
    {
        uint64_t user_id;
        std::string name = "NO_NAME";
    };

    // Определение web_socket с использованием UserData
    typedef uWS::WebSocket<false, true, UserData> web_socket;

    //web_socket* ws_;
    //UserData* data_ =ws_->getUserData();

    std::uint64_t cnt_user_ = 0;
    std::uint16_t port_ = 0;

public:
    explicit Server(const uint16_t port) : cnt_user_(1), port_(port)
    {
    }

    static void ProcessSetName(web_socket* WS, json parsed, UserData* data);

    static void ProcessPrivateMessage(web_socket* WS,  json parsed, std::uint64_t user_id);

    static void ProcessPublicMessage(web_socket* WS,  json parsed, std::uint64_t user_id);

    static void ProcessMessage(web_socket* WS, std::string_view message);

    void run();

};

