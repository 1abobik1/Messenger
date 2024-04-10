#pragma once

#include <uwebsockets/App.h>
#include <nlohmann/json.hpp>

#include "ServiceFunc.h"
#include "DataBase.h"

using json = nlohmann::json;

class Server : protected ServiceFunc
{
private:

    struct UserData
    {
        uint64_t user_id;
        std::string name = "NO_NAME";
    };

    typedef uWS::WebSocket<true, true, UserData> web_socket;

    std::uint64_t cnt_user_;
    std::uint16_t port_ = 0;

public:
    explicit Server(const uint16_t port) : cnt_user_(1), port_(port)
    {
    }

    static void HandleSignUp(uWS::HttpResponse<true>* res, uWS::HttpRequest* req);

    static void HandleLogIn(uWS::HttpResponse<true>* res, uWS::HttpRequest* req);

    static void ProcessSetName(web_socket* WS, json parsed, UserData* data);

    static void ProcessPrivateMessage(web_socket* WS,  json parsed, std::uint64_t user_id);

    static void ProcessPublicMessage(web_socket* WS,  json parsed, std::uint64_t user_id);

    static void ProcessMessage(web_socket* WS, std::string_view message);

    void run();

};

