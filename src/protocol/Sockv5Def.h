#ifndef __H_SOCK5_DEF__
#define __H_SOCK5_DEF__

#include <stdint.h>
#include <string>
#include <array>
#include <set>

const static uint8_t SOCK_V5_VER = 0x5;

enum Sockv5AuthType {
	SOCKV5_NO_AUTH = 0x0,
	SOCKV5_GSSAPI = 0x1,
	SOCKV5_USR_PWD = 0x2,
	SOCKV5_IANA_RESERVE = 0x3,
	SOCKV5_PRI_RESERVE = 0x4,
	SOCKV5_NO_ACCEPT = 0x5
};

struct Sockv5HandShakeReq 
{
public:
	int32_t check(const std::string &data);
	int32_t parseFrom(const std::string &data);

public:
	uint8_t ver;
	std::set<uint8_t> methods;

public:
	std::string toString() const;
};

struct Sockv5Req
{
public:
	int32_t check(const std::string &data);
	int32_t parseFrom(const std::string &data);

public:
	std::string toString() const;

public:
	uint8_t ver;
	uint8_t cmd;
	uint8_t rsv;
	uint8_t atyp;
	uint32_t ipv4;
	std::string host;
	uint16_t port;
	std::array<unsigned char, 16> ipv6;
};

#endif