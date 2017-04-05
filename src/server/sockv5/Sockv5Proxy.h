#pragma once

#include <string>
#include <stdint.h>
#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>
#include "pool/IoServicePool.h"
#include "Session.h"

class Sockv5Proxy : public boost::noncopyable
{
public:
	Sockv5Proxy(const std::string &ip, uint16_t port);
	~Sockv5Proxy();

public:
	void init(int argc, const char *argv[]);
	void start();

protected:
	void handleAccept(boost::shared_ptr<boost::asio::ip::tcp::socket> psocket, boost::system::error_code ec);

private:
	std::string ip_;
	uint16_t port_;
	boost::asio::io_service service_;
	IoServicePool pool_;
	boost::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor_;
};

