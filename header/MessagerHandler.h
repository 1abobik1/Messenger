#pragma once

#include <uwebsockets/WebSocket.h>
#include <nlohmann/json.hpp>

#include "UserModel.h"
#include "WebSocketUser.h"
#include "DataBase.h"
#include "RequestHandler.h"

using json = nlohmann::json;

class MessagerHandler {
private:
    const int MAX_MESSAGE_COUNT = 8;
    const int MIN_SPAM_TIME = 10;

    RequestHandler* request_handler_;

    typedef uWS::WebSocket<false, true, WebSocketUser> web_socket;

    void ConnectedUser(web_socket* WS);

    bool CheckAndHandleBlock(web_socket* WS);
    void RecordMessageTime(web_socket* WS);
    bool CheckAndHandleSpam(web_socket* WS);
    void SendMsg(web_socket* WS);

    void ProcessPrivateMessage(web_socket* WS, json parsed);

    void ProcessMessage(web_socket* WS, std::string_view message);

    void DisconnectedUser(web_socket* WS, int code, std::string_view message);

public:
    MessagerHandler(RequestHandler* request_handler) : request_handler_(request_handler) {}

    friend class Server;
};
