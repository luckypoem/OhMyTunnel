#pragma once

#include "Logger.h"

#define SF_LOG_WITH_ADDRESS(lv, fmt, ...) SF_LOG(lv, "<Sock:%d, rSock:%d, user:%s, address:%s, port:%d>" fmt, (int)sock_->native_handle(), (int)rsock_->native_handle(), this->getUser().c_str(), this->getAddress().c_str(), this->getPort(), ##__VA_ARGS__ )
#define SF_LOG_WITH_CLIENT(lv, fmt, ...) SF_LOG(lv, "<Sock:%d, address:%s:%d>" fmt, (int)sock_->native_handle(), sock_->remote_endpoint().address().to_string().c_str(), (int)sock_->remote_endpoint().port(), ##__VA_ARGS__)
