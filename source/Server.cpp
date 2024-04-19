#define _CRT_SECURE_NO_WARNINGS

#include "../header/Server.h"

#include <iostream>

void Server::run()
{
	ssl_app_
		.post("/signup", [&](auto* res, auto* req) {
			HandleSignUp(res, req);

		}).post("/login", [&](auto* res, auto* req){
			HandleLogIn(res, req);

		}).get("/", [this](auto* res, auto* req){
			RegPanelHTML(res, req);
		}).get("/style.css", [this](auto* res, auto* req){
			RegPanelCSS(res, req);
		}).get("/signup.js", [this](auto* res, auto* req) {
			RegPanelSignupJS(res, req);
		}).get("/login.js", [this](auto* res, auto* req) {
			RegPanelLoginJS(res, req);
		})

		.ws<UserData>("/*",{
		.idleTimeout = 666,

		.open = [this](auto* ws){
			ConnectedUser(ws);
		},
		.message = [this](auto* ws, std::string_view message, uWS::OpCode){
			ProcessMessage(ws,message);
		},
		.close = [this](auto* ws, int code, std::string_view message){
			DisconnectedUser(ws,code,message);
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
