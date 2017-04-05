//
// Created by sen on 2017/4/4.
//

#ifndef OHMYTUNNEL_HTTPREQ_H
#define OHMYTUNNEL_HTTPREQ_H

#include "HttpFrame.h"

class HttpClientFrame : public HttpFrame
{
public:
    HttpClientFrame(const std::string &host) : HttpFrame(host, "http_client") {}
    virtual std::string &addFrame(std::string &data) override;
    virtual std::string &removeFrame(std::string &data) override;
};


#endif //OHMYTUNNEL_HTTPREQ_H
