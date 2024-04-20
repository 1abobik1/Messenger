#pragma once

#include <nlohmann/json.hpp>
#include <uwebsockets/HttpResponse.h>

using json = nlohmann::json;

class RequestHandler
{
private:
    static void HandleSignUp(uWS::HttpResponse<true>* res, uWS::HttpRequest* req);

    static void HandleLogIn(uWS::HttpResponse<true>* res, uWS::HttpRequest* req);
public:
    friend class Server;
};