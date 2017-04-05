//
// Created by sen on 2017/4/4.
//

#include <boost/lexical_cast.hpp>
#include "HttpServerFrame.h"

std::string &HttpServerFrame::removeFrame(std::string &data)
{
    return HttpFrame::removeFrame(data); //post only..
}

std::string &HttpServerFrame::addFrame(std::string &data)
{
    std::string tmp;
    //
    tmp += "HTTP/1.1 200 OK\r\n";
    tmp += "Content-Type: text/html; charset=utf-8\r\n";
    tmp += "Content-Length: " + boost::lexical_cast<std::string>(data.size()) + "\r\n";
    tmp += "Connection: keep-alive\r\n";
    tmp += "\r\n";
    tmp += data;
    //
    tmp.swap(data);
    return data;
}
