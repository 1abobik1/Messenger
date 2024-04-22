#pragma once

#include <nlohmann/json.hpp>
#include <uwebsockets/HttpResponse.h>

using json = nlohmann::json;

class RequestHandler
{
private:

	void HandleSignUp(uWS::HttpResponse<true>* res, uWS::HttpRequest* req);

	void HandleLogIn(uWS::HttpResponse<true>* res, uWS::HttpRequest* req);

public:
    friend class Server;
};