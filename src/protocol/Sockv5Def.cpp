#include "Sockv5Def.h"
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>

/*
uint8_t ver;
uint8_t cmd;
uint8_t rsv;
uint8_t atyp;
uint32_t ipv4;
std::string host;
uint8_t ipv6[16];
*/

int32_t Sockv5Req::parseFrom(const std::string &data)
{
	int32_t size = check(data);
	if(size == 0)
		return 0;
	ver = data.at(0);
	cmd = data.at(1);
	rsv = data.at(2);
	atyp = data.at(3);
	if (atyp == 0x1)
		memcpy(&ipv4, &data[4], 4);
	else if (atyp == 0x3)
	{
		host = data.substr(5, data[4]);
		std::string portStr = data.substr(5 + data[4], 2);
		uint16_t tmp = *reinterpret_cast<const uint16_t *>(portStr.c_str());
		port = boost::asio::detail::socket_ops::network_to_host_short(tmp);
	}
	else
		memcpy(&this->ipv6[0], &data[4], 16);
	return size;
}

int32_t Sockv5Req::check(const std::string &data)
{
	if (data.size() <= 5)
		return 0;
	int32_t len = 4;
	if (data.at(3) == 0x1)
		len += 4;
	else if (data.at(3) == 0x4)
		len += 16;
	else if (data.at(3) == 0x3)
	{
		len += 1 + data.at(4) + 2; //1bytes len, n bytes host, 2bytes port
	}
	if ((signed)data.size() < len)
		return 0;
	return len;
}

std::string Sockv5Req::toString() const
{
	std::string str = "Sockv5Req:[";
	str += "ver:" + boost::lexical_cast<std::string>(ver) + ", ";
	str += "cmd:" + boost::lexical_cast<std::string>(cmd) + ", ";
	str += "rsv:" + boost::lexical_cast<std::string>(rsv) + ", ";
	str += "atyp:" + boost::lexical_cast<std::string>(atyp) + ", ";
	switch (atyp)
	{
	case 0x1:
		str += "ipv4:" + boost::asio::ip::address_v4(this->ipv4).to_string();
		break;
	case 0x3:
		str += "host:" + host + ", " + "port:" + boost::lexical_cast<std::string>(port);
		break;
	case 0x4:
		str += "ipv6:" + boost::asio::ip::address_v6(this->ipv6).to_string();
		break;
	default:
		break;
	}
	str += "]";
	return str;
}

int32_t Sockv5HandShakeReq::check(const std::string &data)
{
	do {
		if (data.size() < 2)
			break;
		int32_t len = 2 + data.at(1);
		if ((signed)data.size() < len)
			return 0;
		return len;

	} while (false);
	return 0;
}

int32_t Sockv5HandShakeReq::parseFrom(const std::string &data)
{
	int32_t len = check(data);
	if (len == 0)
		return 0;
	ver = data.at(0);
	for (int i = 0; i < data.at(1); ++i)
		methods.insert(data.at(i + 2));
	return len;

}