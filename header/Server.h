#pragma once

#include <uwebsockets/App.h>
#include <nlohmann/json.hpp>

#include "DataBase.h"

using json = nlohmann::json;

class Server
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

    static void HandleSignUp(uWS::HttpResponse<true>* res, uWS::HttpRequest* req);

    static void HandleLogIn(uWS::HttpResponse<true>* res, uWS::HttpRequest* req);

    static void RegPanelHTML(uWS::HttpResponse<true>* res, uWS::HttpRequest* req);

    static void RegPanelCSS(uWS::HttpResponse<true>* res, uWS::HttpRequest* req);

    static void RegPanelLoginJS(uWS::HttpResponse<true>* res, uWS::HttpRequest* req);

    static void RegPanelSignupJS(uWS::HttpResponse<true>* res, uWS::HttpRequest* req);

    static void ProcessSetName(web_socket* WS, json parsed, UserData* data);

    static void ProcessPrivateMessage(web_socket* WS,  json parsed, std::uint64_t user_id);

    static void ProcessPublicMessage(web_socket* WS,  json parsed, std::uint64_t user_id);

    static void ProcessMessage(web_socket* WS, std::string_view message);

public:

    explicit Server(const uint16_t port) : cnt_user_(1), port_(port){}

	void run();
};

