//
// Created by sen on 2017/3/25.
//

#ifndef OHMYTUNNEL_SETTING_H
#define OHMYTUNNEL_SETTING_H

#include <string>
#include <stdint.h>
#include <map>

class Setting
{
public:
    uint32_t threadCount;
    std::string localBindAddr;
    uint32_t localListenPort;
    uint32_t serverListenPort;
    std::string serverBindAddr;
    std::string encryptMethod;
    uint32_t timeout;
	std::string key;
	std::string frame;
	int32_t logLv;
	std::map<std::string, std::string> users;

public:
    bool loadConfig(const std::string &file);
    std::string toString() const;
};


#endif //OHMYTUNNEL_SETTING_H
