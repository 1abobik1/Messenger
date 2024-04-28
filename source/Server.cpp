#define _CRT_SECURE_NO_WARNINGS

#include "../header/Server.h"
#include "../header/UserData.h"

#include<uwebsockets/App.h>

#include <iostream>

void Server::run()
{
	uWS::SSLApp({
	.key_file_name = "../misc/key.pem",
	.cert_file_name = "../misc/cert.pem",
	.passphrase = "dima15042004"
	})
		.post("/signup", [&](auto* res, auto* req) {
			request_handler_->HandleSignUp(res, req);

		}).post("/login", [&](auto* res, auto* req){
			request_handler_->HandleLogIn(res, req);

		}).post("/client/DisplayUsers", [&](auto* res, auto* req) {
			request_handler_->HandledDisplayUsers(res, req);

		}).post("/client/SearchUser", [&](auto* res, auto* req) {
			request_handler_->HandleSearchUser(res, req);
		})

		.ws<UserData>("/*",{
		.compression = uWS::SHARED_COMPRESSOR,
		.maxPayloadLength = 10 * 1024,
		.idleTimeout = 666,
		.maxBackpressure = 1 * 1024 * 1024,

		.open = [this](auto* ws){
			  messager_handler_->ConnectedUser(ws);
		},

		.message = [this](auto* ws, std::string_view message, uWS::OpCode){
			 messager_handler_->ProcessMessage(ws,message);
		},

		.close = [this](auto* ws, int code, std::string_view message){
			messager_handler_->DisconnectedUser(ws,code,message);
		}

		})
		.options("/*", [&](auto* res, auto* req) 
		{
			// for policy CORS and not only
			res->writeHeader("Access-Control-Allow-Origin", "*");
			res->writeHeader("Access-Control-Allow-Headers", "Content-Type");
			res->writeHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
			res->writeStatus("200 OK");
			res->end();
		})
		.listen(this->port_, [&](const auto* listenSocket)
		{
			if (listenSocket) {
				//std::cout << "Listening on port 9000 thread: "<< std::this_thread::get_id() << '\n';
			}
			else {
				std::cout << "Failed to load certificates or bind to port." << '\n';
			}
		}).run();
}
