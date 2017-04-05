//
// Created by sen on 2017/4/4.
//

#include <boost/lexical_cast.hpp>
#include "HttpClientFrame.h"
#include "utils/RandomUtils.h"

std::string &HttpClientFrame::addFrame(std::string &data)
{
    std::string tmp;
    //
    tmp += "POST /" + RandomUtils::randStringVisible(RandomUtils::randInt(4, 10)) + " HTTP/1.1\r\n";
    tmp += "Host: " + getHost() + "\r\n";
    tmp += "Connection: keep-alive\r\n";
    tmp += "Content-Length: " + boost::lexical_cast<std::string>(data.size()) + "\r\n";
    tmp += "Pragma: no-cache\r\n";
    tmp += "Cache-Control: no-cache\r\n";
    tmp += "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n";
    tmp += "Content-Type: application/x-www-form-urlencoded\r\n";
    tmp += "User-Agent: Mozilla/5.0 (Windows NT 6.3; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/41.0.2272.87 Safari/537.36\r\n";
    tmp += "\r\n";
    tmp += data;
    //
    data.swap(tmp);
    return data;
}

std::string &HttpClientFrame::removeFrame(std::string &data)
{
    return HttpFrame::removeFrame(data);
}
