#pragma once

#include <nlohmann/json.hpp>
#include <uwebsockets/HttpResponse.h>

#include "UserModel.h"

using json = nlohmann::json;

class RequestHandler{
private:
	std::unique_ptr<UserModel> user_model_;

	void HandleSignUp(uWS::HttpResponse<true>* res, uWS::HttpRequest* req);

	void HandleLogIn(uWS::HttpResponse<true>* res, uWS::HttpRequest* req);

	//void HandledDisplayUsers(uWS::HttpResponse<true>* res, uWS::HttpRequest* req);

	void HandleSearchUser(uWS::HttpResponse<true>* res, uWS::HttpRequest* req);
public:
	UserModel* getUserModel() const;

	RequestHandler() : user_model_(std::make_unique<UserModel>()){}

    friend class Server;
};