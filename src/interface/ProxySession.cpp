//
// Created by sen on 2017/4/4.
//

#include "ProxySession.h"

ProxySession::ProxySession(const std::string &name)
    : name_(name), port_(0)
{

}

void ProxySession::setUser(const std::string &user)
{
    user_ = user;
}

const std::string &ProxySession::getUser() const
{
    return user_;
}

void ProxySession::setAddress(const std::string &address)
{
    address_ = address;
}

const std::string &ProxySession::getAddress() const
{
    return address_;
}

void ProxySession::setPort(uint32_t port)
{
    port_ = port;
}

uint32_t ProxySession::getPort() const
{
    return port_;
}

const std::string &ProxySession::getName() const
{
    return name_;
}

const std::string &ProxySession::getPassword() const
{
    return pwd_;
}

void ProxySession::setPassword(const std::string &pwd)
{
    pwd_ = pwd;
}
