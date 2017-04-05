#pragma once

#include <boost/asio.hpp>
#include "protos/ProxyMsg.pb.h"
#include <string>

enum ParseCode { PROXY_PARSE_OK = 0, PROXY_PB_FAIL, PROXY_CHECK_SUM_ERR, PROXY_ERRCODE_NOT_ZERO, PROXY_BODY_EMPTY };

class ProxyMsgPkg
{
public:
	ProxyMsgPkg();
	~ProxyMsgPkg();

public:
	int32_t parseAndCheck(const std::string &data);
	bool serializedToString(std::string &data);
	const std::string &lastErrorMsg() const { return lastErr_; }

	ProxyMsgPkg &setCmd(uint32_t cmd);
	ProxyMsgPkg &setSubCmd(uint32_t subcmd);
	proxy::msg::ProxyMsg &package() { return req_; }

public:
	ProxyMsgPkg &createConnectReq(uint32_t type, const std::string &host, uint16_t port, const std::string &user,
                                  const std::string &pwd);
	ProxyMsgPkg &createDataPkg(const std::string &data);
	ProxyMsgPkg &createCommonRsp(ProxyMsgPkg &req, const std::string &body, uint32_t errcode);

protected:
	void createDefaultParams();

private:
	proxy::msg::ProxyMsg req_;
	std::string lastErr_;
};

