#define _CRT_SECURE_NO_WARNINGS

#include "../header/FileSender.h"
#include "../header/Server.h"

#include<fstream>
#include <future>

void FileSender::RegPanelHTML(uWS::HttpResponse<true>* res, uWS::HttpRequest* req)
{
	res->writeHeader("Content-Type", "text/html");

	auto future_content = std::async(std::launch::async, [&]() {
		std::ifstream file_path("C:/Users/dima1/source/repos/ChatServer/web-files/index.html");
		if (file_path) {
			return std::string((std::istreambuf_iterator<char>(file_path)), std::istreambuf_iterator<char>());
		}
		return std::string("File not found");
		});

	res->end(future_content.get());
}

void FileSender::RegPanelCSS(uWS::HttpResponse<true>* res, uWS::HttpRequest* req)
{
	res->writeHeader("Content-Type", "text/css");

	auto future_content = std::async(std::launch::async, [&]() {
		std::ifstream file_path("C:/Users/dima1/source/repos/ChatServer/web-files/style.css");
		if (file_path) {
			return std::string((std::istreambuf_iterator<char>(file_path)), std::istreambuf_iterator<char>());
		}
		return std::string("File not found");
		});

	res->end(future_content.get());
}

void FileSender::RegPanelLoginJS(uWS::HttpResponse<true>* res, uWS::HttpRequest* req)
{
	res->writeHeader("Content-Type", "application/js");

	auto future_content = std::async(std::launch::async, [&]() {
		std::ifstream file_path("C:/Users/dima1/source/repos/ChatServer/web-files/login.js");
		if (file_path) {
			return std::string((std::istreambuf_iterator<char>(file_path)), std::istreambuf_iterator<char>());
		}
		return std::string("File not found");
		});

	res->end(future_content.get());
}

void FileSender::RegPanelSignupJS(uWS::HttpResponse<true>* res, uWS::HttpRequest* req)
{
	res->writeHeader("Content-Type", "application/js");

	auto future_content = std::async(std::launch::async, [&]() {
		std::ifstream file_path("C:/Users/dima1/source/repos/ChatServer/web-files/signup.js");
		if (file_path) {
			return std::string((std::istreambuf_iterator<char>(file_path)), std::istreambuf_iterator<char>());
		}
		return std::string("File not found");
		});

	res->end(future_content.get());
}

