//
// Created by sen on 2017/3/25.
//

#include "Setting.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
//#include <boost/log/support/date_time.hpp>
//#include <boost/log/trivial.hpp>
#include "utils/Logger.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <boost/lexical_cast.hpp>

bool Setting::loadConfig(const std::string &file)
{
    boost::property_tree::ptree pt;
    try
    {
        boost::property_tree::json_parser::read_json<boost::property_tree::ptree>(file, pt);
    }
    catch (boost::property_tree::ptree_error & e)
    {
        SF_LOG(LOG_ERROR, "Read json file:%s failed, err:%s", file.c_str(), e.what());
        return false;
    }
    threadCount = pt.get<uint32_t>("threads", 1);
    localBindAddr = pt.get<std::string>("local_bind", "127.0.0.1");
    serverBindAddr = pt.get<std::string>("server_bind", "127.0.0.1");
    localListenPort = pt.get<uint32_t>("local_port", 2333);
    serverListenPort = pt.get<uint32_t>("server_port", 8000);
    encryptMethod = pt.get<std::string>("method", "xor");
    timeout = pt.get<uint32_t>("timeout", 5); //second
	key = pt.get<std::string>("key", "12345");
    frame = pt.get<std::string>("frame", "fix");
    logLv = pt.get<uint32_t>("loglv", 0);
	return true;
}

namespace std {
    std::string to_string(const std::string &str) {
        return str;
    }
}

//#ifdef _WIN32
//namespace std {
//    std::string to_string(uint32_t value) {
//        std::stringstream ss;
//        ss << value;
//        return ss.str();
//    }
//}
//#endif

#define TOSTRING(name) do { \
    buf += #name; \
    buf += ":"; \
    buf += boost::lexical_cast<std::string>(name); \
    buf += ", "; \
} while(false)
std::string Setting::toString() const
{
    std::string buf = "Setting:[";
    TOSTRING(threadCount);
    TOSTRING(localBindAddr);
    TOSTRING(localListenPort);
    TOSTRING(serverBindAddr);
    TOSTRING(serverListenPort);
    TOSTRING(encryptMethod);
    TOSTRING(timeout);
    TOSTRING(key);
    TOSTRING(frame);
    TOSTRING(logLv);
    buf += "users:{";
    for(auto &user : users)
    {
        buf += user.first + ":" + user.second + ",";
    }
    if(!users.empty())
        buf.pop_back();
    buf += "}";
    buf += "]";
    return buf;
}
