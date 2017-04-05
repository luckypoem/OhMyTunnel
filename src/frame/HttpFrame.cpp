//
// Created by sen on 2017/4/3.
//

#include "HttpFrame.h"
#include <boost/format.hpp>     
#include <boost/tokenizer.hpp>     
#include <boost/algorithm/string.hpp> 
#include <boost/lexical_cast.hpp>

int32_t HttpFrame::check(const std::string &data)
{
	if(data.size() <= 4)
		return 0;
	//
    std::string type = data.substr(0, 4);
    if(type.back() == ' ')
        type.pop_back();
    if(type == "POST")  //post or rsp
        return checkPost(data);
    else if(type == "HTTP")
        return checkRsp(data);
    else if(type == "GET")
        return checkGet(data);
    return unknowCheck(data);
}

std::string &HttpFrame::removeFrame(std::string &data)
{
	int32_t pos = data.find("\r\n\r\n");
	assert(pos != std::string::npos);
	data.substr(pos + 4).swap(data);
    return data;
}

int32_t HttpFrame::checkGet(const std::string &data)
{
    int32_t len = data.find("\r\n\r\n");
    if(len == std::string::npos)
        return 0;
    return len + 4;
}

int32_t HttpFrame::checkPost(const std::string &data)
{
    size_t headerLen = data.find("\r\n\r\n");
    if(headerLen == std::string::npos)
        return 0;
    size_t bodyLenIndex = data.find("Content-Length: ");
    if(bodyLenIndex == std::string::npos)
        return -1;  //not found content-length;
    if(bodyLenIndex > headerLen)
        return -2;  //invalid position
    size_t bodyLenEndIndex = data.find("\r\n", bodyLenIndex);
    if(bodyLenEndIndex == std::string::npos || bodyLenEndIndex > headerLen)
        return -3; //invalid end
    std::string contentLength = data.substr(bodyLenIndex, bodyLenEndIndex - bodyLenIndex);
    size_t posIndex = contentLength.find(": ");
    contentLength = contentLength.substr(posIndex + 2);
    try
    {
        int32_t len = boost::lexical_cast<int32_t>(contentLength) + headerLen + 4;
        if(data.size() < len)
            return 0;
        return len;
    }
    catch(std::exception &e)
    {
        return -4; //exception!!
    }
}

int32_t HttpFrame::checkRsp(const std::string &data)
{
    return checkPost(data);
}
