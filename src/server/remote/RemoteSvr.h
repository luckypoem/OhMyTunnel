#pragma once

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include "pool/IoServicePool.h"

class RemoteSvr : public boost::noncopyable
{
public:
	RemoteSvr(const std::string &ip, uint16_t port);
	~RemoteSvr();

public:
	void start();

protected:
	void handleAccept(boost::shared_ptr<boost::asio::ip::tcp::socket> sock, boost::system::error_code ec);

private:
	std::string ip_;
	uint16_t port_;
	boost::asio::io_service service_;
	boost::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor_;
	IoServicePool pool_;
};

