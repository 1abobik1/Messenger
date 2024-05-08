#define _CRT_SECURE_NO_WARNINGS

#include "../header/Server.h"
#include "../header/UserModel.h"

#include<uwebsockets/App.h>

#include <iostream>

void Server::run()
{
	uWS::App()
		.post("/signup", [&](auto* res, auto* req) {
			request_handler_->HandleSignUp(res, req);

		}).post("/login", [&](auto* res, auto* req){
			request_handler_->HandleLogIn(res, req);

		}).post("/client/PrintMessages", [&](auto* res, auto* req) {
			request_handler_->HandleClientsMessages(res, req);

		}).post("/client/SearchUser", [&](auto* res, auto* req) {
			request_handler_->HandleSearchUser(res, req);
		})

		.ws<WebSocketUser>("/*",{
			.maxPayloadLength = 16 * 1024,
			.idleTimeout = 120,
			.resetIdleTimeoutOnSend = true,

			.open = [&](auto* ws){
				  messager_handler_->ConnectedUser(ws);
			},

			.message = [&](auto* ws, std::string_view message, uWS::OpCode){
				 messager_handler_->ProcessMessage(ws,message);
			},

			.pong = [](auto* ws, std::string_view message) {
				std::cout << " ---pong--- " << message << '\n';
			},

			.close = [&](auto* ws, int code, std::string_view message){
				messager_handler_->DisconnectedUser(ws,code,message);
			}
			
			})
			.options("/*", [&](auto* res, auto* req) 
			{
				// for policy CORS and not only
				res->writeHeader("Access-Control-Allow-Origin", "*");
				res->writeHeader("Access-Control-Allow-Headers", "Content-Type");
				res->writeHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
				res->end();
			})
			.listen(this->port_, [&](const auto* listenSocket)
			{
				if (listenSocket) {
					std::cout << "Server is running" << '\n';
				}
				else {
					std::cout << "Server Failed" << '\n';
				}
			}).run();
}
