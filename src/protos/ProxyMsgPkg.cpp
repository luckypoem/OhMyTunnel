#include "ProxyMsgPkg.h"
#include "utils/RandomUtils.h"
#include "utils/CRC32.h"
#include <boost/lexical_cast.hpp>

ProxyMsgPkg::ProxyMsgPkg()
{
}


ProxyMsgPkg::~ProxyMsgPkg()
{
}

int32_t ProxyMsgPkg::parseAndCheck(const std::string &data)
{
	if(!req_.ParseFromString(data))
    {
        lastErr_ = "PB parse failed!";
        return PROXY_PB_FAIL;
    }
    if(req_.err_code() != 0)
    {
        lastErr_ = "Ret code != 0, code:" + boost::lexical_cast<std::string>(data.size());
        return PROXY_ERRCODE_NOT_ZERO;
    }
    if(!req_.has_body() || req_.body().empty())
    {
        lastErr_ = "Body empty";
        return PROXY_BODY_EMPTY;
    }
    int32_t checksum = CRC32::calculate(req_.body().c_str(), req_.body().size());
    if(checksum != req_.check_sum())
    {
        lastErr_ = "Checksum failed, head checksum:" + boost::lexical_cast<std::string>(req_.check_sum()) + ", body checksum:" + boost::lexical_cast<std::string>(checksum);
        return PROXY_CHECK_SUM_ERR;
    }
    return PROXY_PARSE_OK;
}

bool ProxyMsgPkg::serializedToString(std::string & data)
{
	return req_.SerializeToString(&data);
}

ProxyMsgPkg & ProxyMsgPkg::setCmd(uint32_t cmd)
{
	req_.set_cmd(cmd);
	return *this;
}

ProxyMsgPkg & ProxyMsgPkg::setSubCmd(uint32_t subcmd)
{
	req_.set_subcmd(subcmd);
	return *this;
}

ProxyMsgPkg & ProxyMsgPkg::createConnectReq(uint32_t type, const std::string &host, uint16_t port,
                                            const std::string &user, const std::string &pwd)
{
	proxy::msg::ProxyConnectReq req;
	req.set_address(host);
	req.set_port(port);
	req.set_type(type);
	req.set_username(user);
	req.set_password(pwd);
	std::string str;
	req.SerializeToString(&str);
	req_.set_cmd(proxy::msg::ServerCmd);
	req_.set_subcmd(proxy::msg::SubCmd0x1);
	req_.set_body(str);
	req_.set_err_code(0);
	createDefaultParams();
	return *this;
}

ProxyMsgPkg &ProxyMsgPkg::createDataPkg(const std::string & data)
{
	req_.set_cmd(proxy::msg::ServerCmd);
	req_.set_subcmd(proxy::msg::SubCmd0x2);
	req_.set_body(data);
	req_.set_err_code(0);
	createDefaultParams();
	return *this;
}

ProxyMsgPkg & ProxyMsgPkg::createCommonRsp(ProxyMsgPkg & req, const std::string & body, uint32_t errcode)
{
	req_.set_cmd(req.package().cmd());
	req_.set_subcmd(req.package().subcmd());
	req_.set_seq(req.package().seq());
	req_.set_body(body);
	req_.set_err_code(errcode);
	createDefaultParams();
	return *this;
}

void ProxyMsgPkg::createDefaultParams()
{
	static uint32_t seq = 0;
	if(!req_.has_seq())
		req_.set_seq(seq++);
	req_.set_check_sum(0);
	if(req_.has_body())
	{
		req_.set_check_sum(CRC32::calculate(req_.body().c_str(), req_.body().size()));
	}
	req_.set_place_holder(RandomUtils::randString(RandomUtils::randInt(2, 10)));
}