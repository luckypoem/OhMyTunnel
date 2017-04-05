//
// Created by sen on 2017/4/3.
//

#include "FixLengthFrame.h"
#include <boost/asio.hpp>

int32_t FixLengthFrame::check(const std::string &data)
{
    uint32_t len = *reinterpret_cast<const uint32_t *>(data.c_str());
    len = boost::asio::detail::socket_ops::network_to_host_long(len);
    if (len + 4 > data.size())
        return 0;
    return len + 4;
}

std::string &FixLengthFrame::removeFrame(std::string &data)
{
    uint32_t len = *reinterpret_cast<const uint32_t *>(data.c_str());
    len = boost::asio::detail::socket_ops::network_to_host_long(len);
    data.substr(4, len).swap(data);
    return data;
}

std::string &FixLengthFrame::addFrame(std::string &data)
{
    uint32_t len = data.size();
    len = boost::asio::detail::socket_ops::host_to_network_long(len);
    std::string tmp((const char *)&len, 4);
    tmp.append(data);
    tmp.swap(data);
    return data;
}
