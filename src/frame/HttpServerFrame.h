//
// Created by sen on 2017/4/4.
//

#ifndef OHMYTUNNEL_HTTPSERVER_H
#define OHMYTUNNEL_HTTPSERVER_H

#include "HttpFrame.h"

class HttpServerFrame : public HttpFrame
{
public:
    HttpServerFrame(const std::string &host) : HttpFrame(host, "http_server") {}
    virtual std::string &removeFrame(std::string &data) override;
    virtual std::string &addFrame(std::string &data) override;
};


#endif //OHMYTUNNEL_HTTPSERVER_H
