//
// Created by sen on 2017/3/26.
//

#ifndef OHMYTUNNEL_SESSION_H
#define OHMYTUNNEL_SESSION_H

#include <boost/shared_ptr.hpp>
#include <string>
#include <stdint.h>
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_array.hpp>
#include "protocol/Sockv5Def.h"
#include "interface/ProxySession.h"

class Session : public boost::enable_shared_from_this<Session>, ProxySession
{
private:
	const static int SESSION_BUFFER_LEN = 16 * 1024;

public:
	Session(boost::shared_ptr<boost::asio::ip::tcp::socket> sock);
	~Session();

public:
	void start();
	Session &setAuthInfo(const std::string &user, const std::string &pwd) { this->setUser(user);
        this->setPassword(pwd); return *this; }
	Session &setRemoteSvr(const std::string &svr, uint32_t port) { this->remoteSvr_ = svr, this->remotePort_ = port; return *this; }

protected:
	void handleReadHandShake(const boost::system::error_code &error, size_t bytes);
	void handleWriteHandShake(const boost::system::error_code &error, size_t bytes);
	void handleReadSockv5Req(const boost::system::error_code &error, size_t bytes);
	void handleConnectToRemote(const boost::system::error_code &error);
	void handleReadAuthRspFromRemote(const boost::system::error_code &error, size_t bytes);
	void handleWriteSockv5RspToBrowser(const boost::system::error_code &error, size_t bytes);
	void handleBeginPipeTransfer(const boost::system::error_code &error, size_t bytes);
	void handleReadDataFromBrowser(const boost::system::error_code &error, size_t bytes);
	void handleWriteDataToRemote(const boost::system::error_code &error, size_t bytes);
	void handleReadDataFromRemote(const boost::system::error_code &error, size_t bytes);
	void handleWriteDataToBrowser(const boost::system::error_code &error, size_t bytes);

protected:
	void shutdown();

protected:
	const std::string &getRemoteSvr() const { return remoteSvr_; }
	uint32_t getRemotePort() const { return remotePort_; }

private:
	boost::shared_ptr<boost::asio::ip::tcp::socket> sock_;
	boost::shared_ptr<boost::asio::ip::tcp::socket> rsock_;
	Sockv5HandShakeReq handshakeData_;
	Sockv5Req reqData_;
	std::array<char, SESSION_BUFFER_LEN> bbuffer_; //�����buffer
	std::array<char, SESSION_BUFFER_LEN * 2> rbuffer_; //Զ��buffer
	std::string tmpBBuffer_; //�������ʱ����
	std::string tmpRBuffer_; //Զ����ʱ����
	std::string sockReqStringData_;
	std::string tmpConnectBuffer_;
	//
	std::string remoteSvr_;
	uint32_t remotePort_;
	//
};

#endif //OHMYTUNNEL_SESSION_H
