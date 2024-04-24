#pragma once


#include "FileSender.h"
#include "MessagerHandler.h"
#include "RequestHandler.h"


class Server
{
private:
	std::uint16_t port_ = 0;

	std::unique_ptr<RequestHandler> request_handler_;
	std::unique_ptr<FileSender> file_sender_;
	std::unique_ptr<MessagerHandler> messager_handler_;

public:
	explicit Server(const uint16_t port) : port_(port)
	{
		request_handler_ = std::make_unique<RequestHandler>();
		file_sender_ = std::make_unique<FileSender>();
		messager_handler_ = std::make_unique<MessagerHandler>();
	}

	void run();
};

