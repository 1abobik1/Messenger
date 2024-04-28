#pragma once

#include <nlohmann/json.hpp>
#include <uwebsockets/HttpResponse.h>

#include "UserData.h"

using json = nlohmann::json;

class RequestHandler
{
private:
	std::unique_ptr<UserData> UserData_;

	void HandleSignUp(uWS::HttpResponse<true>* res, uWS::HttpRequest* req);

	void HandleLogIn(uWS::HttpResponse<true>* res, uWS::HttpRequest* req);

public:
	RequestHandler(): UserData_(std::make_unique<UserData>()){}

    friend class Server;
};