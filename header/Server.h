#pragma once

#include <uwebsockets/App.h>

#include "FileSender.h"
#include "MessagerHandler.h"
#include "RequestHandler.h"
#include "UserData.h"

class Server : protected RequestHandler, protected FileSender, protected MessagerHandler
{
private:
	uWS::SSLApp ssl_app_;

	std::uint16_t port_ = 0;
public:

    explicit Server(const uint16_t port) : ssl_app_({
		.key_file_name = "../misc/key.pem",
		.cert_file_name = "../misc/cert.pem",
		.passphrase = "dima15042004"
	}),port_(port)
	{
    }

	void run();
};

