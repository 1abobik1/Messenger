#pragma once

#include <uwebsockets/WebSocket.h>
#include <nlohmann/json.hpp>

#include "UserModel.h"
#include "WebSocketUser.h"
#include "DataBase.h"
#include "RequestHandler.h"

using json = nlohmann::json;

class MessagerHandler
{
private:
	RequestHandler* request_handler_;

	typedef uWS::WebSocket<true, true, WebSocketUser> web_socket;

    void ConnectedUser(web_socket* WS);

    std::string ProcessUserStatus(WebSocketUser* data, bool online);

	/*void ProcessSetName(web_socket* WS, json parsed, WebSocketUser* data);*/

	void ProcessPrivateMessage(web_socket* WS, json parsed);

	void ProcessPublicMessage(web_socket* WS, json parsed);

	void ProcessMessage(web_socket* WS, std::string_view message);

    void DisconnectedUser(web_socket* WS, int code, std::string_view message);
public:
	MessagerHandler(RequestHandler* request_handler) : request_handler_(request_handler) {}

	friend class Server;
};
