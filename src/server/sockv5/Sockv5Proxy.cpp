#include "Sockv5Proxy.h"
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include "utils/Logger.h"
#include <boost/make_shared.hpp>
#include "server/sockv5/Setting.h"

extern Setting g_setting;
using namespace boost::asio::ip;

Sockv5Proxy::Sockv5Proxy(const std::string & ip, uint16_t port)
	: ip_(ip), port_(port) 
{
	acceptor_.reset(new boost::asio::ip::tcp::acceptor(service_, tcp::endpoint(boost::asio::ip::address().from_string(ip), port_)));
}

Sockv5Proxy::~Sockv5Proxy()
{
}

void Sockv5Proxy::start()
{
	SF_LOG(LOG_INFO, "Server bind on ip:%s, port:%d", ip_.c_str(), port_);
	pool_.start();

	//
	boost::shared_ptr<tcp::socket> sock(new tcp::socket(*pool_.get()));
	acceptor_->async_accept(*sock, boost::bind(&Sockv5Proxy::handleAccept, this, sock, boost::asio::placeholders::error));

	service_.run();
}

void Sockv5Proxy::handleAccept(boost::shared_ptr<tcp::socket> psocket, boost::system::error_code ec)
{
	if (ec) 
	{
		SF_LOG(LOG_ERROR, "Accept socket failed, error:%d, errmsg:%s", ec.value(), ec.message().c_str());
		return;
	}
	SF_LOG(LOG_INFO, "Accept socket, sock:%d, remote ip:%s, port:%d", (int)psocket->native_handle(), psocket->remote_endpoint().address().to_string().c_str(), psocket->remote_endpoint().port());
	boost::shared_ptr<Session> session(new Session(psocket));
	psocket->set_option(boost::asio::ip::tcp::acceptor::linger(true, 0));
	session->setAuthInfo(g_setting.user, g_setting.password)
		.setRemoteSvr(g_setting.serverBindAddr, g_setting.serverListenPort);
	session->start();

	boost::shared_ptr<tcp::socket> sock(new tcp::socket(*pool_.get()));
	acceptor_->async_accept(*sock, boost::bind(&Sockv5Proxy::handleAccept, this, sock, boost::asio::placeholders::error));
}

