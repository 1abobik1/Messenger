#pragma once

#include <nlohmann/json.hpp>
#include <uwebsockets/HttpResponse.h>

#include "../bcrypt-cpp/include/bcrypt.h"
#include "UserData.h"
#include "DataBase.h"

using json = nlohmann::json;

class RequestHandler
{
protected:
    static void HandleSignUp(uWS::HttpResponse<true>* res, uWS::HttpRequest* req);

    static void HandleLogIn(uWS::HttpResponse<true>* res, uWS::HttpRequest* req);
};