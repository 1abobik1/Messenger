#pragma once

#include <uwebsockets/HttpResponse.h>

#include<fstream>
#include <future>


class FileSender
{
protected:

    static void RegPanelHTML(uWS::HttpResponse<true>* res, uWS::HttpRequest* req);

    static void RegPanelCSS(uWS::HttpResponse<true>* res, uWS::HttpRequest* req);

    static void RegPanelLoginJS(uWS::HttpResponse<true>* res, uWS::HttpRequest* req);

    static void RegPanelSignupJS(uWS::HttpResponse<true>* res, uWS::HttpRequest* req);
};
