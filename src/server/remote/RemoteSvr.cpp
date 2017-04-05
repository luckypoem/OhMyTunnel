#include "RemoteSvr.h"
#include <boost/bind.hpp>
#include "utils/Logger.h"
#include "RemoteSession.h"
#include "server/remote/Setting.h"

extern Setting g_setting;

RemoteSvr::RemoteSvr(const std::string &ip, uint16_t port)
	: ip_(ip), port_(port)
{
	acceptor_.reset(new boost::asio::ip::tcp::acceptor(service_, boost::asio::ip::tcp::endpoint(boost::asio::ip::address().from_string(ip), port_)));
}


RemoteSvr::~RemoteSvr()
{
}

void RemoteSvr::start()
{
	SF_LOG(LOG_INFO, "Server bind on ip:%s, port:%d", ip_.c_str(), port_);

	pool_.start();

	boost::shared_ptr<boost::asio::ip::tcp::socket> sock(new boost::asio::ip::tcp::socket(*pool_.get()));
	acceptor_->async_accept(*sock, boost::bind(&RemoteSvr::handleAccept, this, sock, boost::asio::placeholders::error));

	service_.run();
}

void RemoteSvr::handleAccept(boost::shared_ptr<boost::asio::ip::tcp::socket> sock, boost::system::error_code ec)
{
	if (ec)
	{
		SF_LOG(LOG_ERROR, "Accept socket failed, error:%d, errmsg:%s", ec.value(), ec.message().c_str());
		return;
	}
	SF_LOG(LOG_INFO, "Accept socket, sock:%d, remote ip:%s, port:%d", (int)sock->native_handle(), sock->remote_endpoint().address().to_string().c_str(), sock->remote_endpoint().port());
	sock->set_option(boost::asio::ip::tcp::acceptor::linger(true, 0));
	boost::shared_ptr<RemoteSession> session(new RemoteSession(sock));
	session->setUserList(g_setting.users);
	session->start();

	boost::shared_ptr<boost::asio::ip::tcp::socket> sockt(new boost::asio::ip::tcp::socket(*pool_.get()));
	acceptor_->async_accept(*sockt, boost::bind(&RemoteSvr::handleAccept, this, sockt, boost::asio::placeholders::error));
}
