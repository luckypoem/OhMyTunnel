//
// Created by sen on 2017/4/4.
//

#ifndef OHMYTUNNEL_SESSIONBASE_H
#define OHMYTUNNEL_SESSIONBASE_H

#include <string>

class ProxySession
{
public:
    ProxySession(const std::string &name);
    virtual ~ProxySession() {};

public:
    void setUser(const std::string &user);
    const std::string &getUser() const;
    void setAddress(const std::string &address);
    const std::string &getAddress() const;
    void setPort(uint32_t port);
    uint32_t getPort() const;
    const std::string &getPassword() const;
    void setPassword(const std::string &pwd);

public:
    const std::string &getName() const;

private:
    std::string name_;
    std::string user_;
    std::string address_;
    std::string pwd_;
    uint32_t port_;
};


#endif //OHMYTUNNEL_SESSIONBASE_H
