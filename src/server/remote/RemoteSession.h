#pragma once

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <string>
#include <array>
#include <boost/enable_shared_from_this.hpp>
#include "protos/ProxyMsgPkg.h"
#include <map>
#include "interface/ProxySession.h"

class RemoteSession : public boost::enable_shared_from_this<RemoteSession>, ProxySession
{
	const static int SESSION_BUFFER_LEN = 16 * 1024;

public:
	RemoteSession(boost::shared_ptr<boost::asio::ip::tcp::socket> sock);
	~RemoteSession();

public:
	void start();

protected:
	void handleAuthFromBrowser(const boost::system::error_code &error, size_t bytes);
	void handleQueryDns(const boost::system::error_code &error, boost::asio::ip::tcp::resolver::iterator ep);
	void handleConnectToTarget(const boost::system::error_code &error);
	void handleWriteRespToBrowser(const boost::system::error_code &error, size_t bytes);
	void handleReadDataFromTarget(const boost::system::error_code &error, size_t bytes);
	void handleWriteDataToBrowser(const boost::system::error_code &error, size_t bytes);
	void handleReadDataFromBrowser(const boost::system::error_code &error, size_t bytes);
	void handleWriteDataToTarget(const boost::system::error_code &error, size_t bytes);

public:
	RemoteSession &setUserList(const std::map<std::string, std::string> &userList) { this->userList_ = userList; return *this; }

protected:
	void shutdown();
	bool checkUser(const std::string &user, const std::string &pwd);

private:
	boost::shared_ptr<boost::asio::ip::tcp::socket> sock_;
	boost::shared_ptr<boost::asio::ip::tcp::socket> rsock_;
	boost::shared_ptr<boost::asio::ip::tcp::resolver> resolver_;
	std::array<char, SESSION_BUFFER_LEN * 2> bbuffer_; //�����buffer
	std::array<char, SESSION_BUFFER_LEN> rbuffer_; //Զ��buffer
	std::string tmpBBuffer_;
	std::string tmpRBuffer_;
	ProxyMsgPkg authPkg_;

	std::map<std::string, std::string> userList_;
};

