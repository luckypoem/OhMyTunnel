//
// Created by sen on 2017/3/26.
//

#include "Session.h"
#include "utils/SessionLogger.h"
#include <boost/bind.hpp>
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/shared_array.hpp>
#include "protos/ProxyMsgPkg.h"
#include "codec/CodecFacade.h"
#include "frame/FrameFacade.h"

static const char SOCK_V5_NO_AUTH_DATA[] = { 0x5, 0x0 };

Session::Session(boost::shared_ptr<boost::asio::ip::tcp::socket> sock)
	: ProxySession("sockv5"), sock_(sock), rsock_(new boost::asio::ip::tcp::socket(sock->get_io_service()))
{
	
}

Session::~Session()
{
	SF_LOG(LOG_DEBUG, "Session destroy, remote address:%s, port:%d", getAddress().c_str(), getPort());
}

void Session::start()
{
	sock_->async_read_some(boost::asio::buffer(bbuffer_),
		boost::bind(&Session::handleReadHandShake, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void Session::handleReadHandShake(const boost::system::error_code &error, size_t bytes)
{
	if (!error)
	{
		tmpBBuffer_ += std::string(bbuffer_.data(), bytes);
		int32_t len = handshakeData_.check(tmpBBuffer_);
		if (len == 0)
		{
			sock_->async_read_some(boost::asio::buffer(bbuffer_),
				boost::bind(&Session::handleReadHandShake, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
			return;
		}
		handshakeData_.parseFrom(tmpBBuffer_);
		tmpBBuffer_ = tmpBBuffer_.substr(len);
		if (handshakeData_.methods.count(Sockv5AuthType::SOCKV5_NO_AUTH) == 0 || handshakeData_.ver != SOCK_V5_VER)
		{
			sock_->close();
			return;
		}
		boost::asio::async_write(*sock_, boost::asio::buffer(SOCK_V5_NO_AUTH_DATA, sizeof(SOCK_V5_NO_AUTH_DATA)),
			boost::bind(&Session::handleWriteHandShake, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	}
	else
	{
		SF_LOG_WITH_CLIENT(LOG_ERROR, "Read hand shake data from browser failed, error:%d, errmsg:%s", error.value(), error.message().c_str());
	}
}

void Session::handleWriteHandShake(const boost::system::error_code &error, size_t bytes)
{
	if (!error)
	{
		SF_LOG_WITH_CLIENT(LOG_TRACE, "Write handle rsp to browser success, begin read sockv5 req!");
		sock_->async_read_some(boost::asio::buffer(bbuffer_),
			boost::bind(&Session::handleReadSockv5Req, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	}
	else
	{
		SF_LOG_WITH_CLIENT(LOG_ERROR, "Write hand shake rsp to browser failed, error:%d, errmsg:%s", error.value(), error.message().c_str());
	}
}

void Session::handleReadSockv5Req(const boost::system::error_code &error, size_t bytes)
{
	if (!error)
	{
		tmpBBuffer_ += std::string(bbuffer_.data(), bytes);
		int32_t len = reqData_.check(tmpBBuffer_);
		if (len == 0)
		{
			sock_->async_read_some(boost::asio::buffer(bbuffer_),
				boost::bind(&Session::handleReadSockv5Req, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
			return;
		}
		reqData_.parseFrom(tmpBBuffer_);
		if (reqData_.cmd != 0x1)
		{
			SF_LOG_WITH_CLIENT(LOG_ERROR, "Handle sockv5 req data, get unsupport cmd:%d", (int)reqData_.cmd);
			sock_->close();
			return;
		}
		rsock_->async_connect(boost::asio::ip::tcp::endpoint(boost::asio::ip::address_v4().from_string(getRemoteSvr()), getRemotePort()),
			boost::bind(&Session::handleConnectToRemote, shared_from_this(), boost::asio::placeholders::error));
		sockReqStringData_ = tmpBBuffer_.substr(0, len); 
		tmpBBuffer_ = tmpBBuffer_.substr(len);
		
		if (!tmpBBuffer_.empty())
		{
			SF_LOG_WITH_CLIENT(LOG_ERROR, "After parse sockv5 req data, buffer remains %d bytes, may err!", (int)tmpBBuffer_.size());
		}
	}
	else
	{
		SF_LOG_WITH_CLIENT(LOG_ERROR, "Read sockv5 req failed, error:%d, errmsg:%s", error.value(), error.message().c_str());
	}
}

void Session::handleConnectToRemote(const boost::system::error_code &error)
{
	if (!error)
	{
		rsock_->set_option(boost::asio::ip::tcp::acceptor::linger(true, 0));
		std::string reqData;
		ProxyMsgPkg req;
		std::string address;
		switch (reqData_.atyp)
		{
		case 0x1:
			address = boost::asio::ip::address_v4(reqData_.ipv4).to_string();
			break;
		case 0x3:
			address = reqData_.host;
			break;
		case 0x4:
			address = boost::asio::ip::address_v6(reqData_.ipv6).to_string();
		}
		//for log
		setAddress(address);
		setPort(reqData_.port);
		//
		std::string &tmp = tmpConnectBuffer_;
		req.createConnectReq(reqData_.atyp, address, reqData_.port, getUser(), getPassword())
			.serializedToString(tmp);
		SF_LOG_WITH_ADDRESS(LOG_TRACE, "Connect to remote server success, write auth data, data size(without header):%d", (int)tmp.size());
        FrameFacade::get_mutable_instance().addFrames(CodecFacade::get_mutable_instance().encode(tmp));

		boost::asio::async_write(*rsock_, boost::asio::buffer(tmp),
			boost::bind(&Session::handleReadAuthRspFromRemote, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	}
	else
	{
		SF_LOG_WITH_ADDRESS(LOG_ERROR, "Connect to remote failed, remotesvr:%s:%d, error code:%d, message:%s", getRemoteSvr().c_str(), getRemotePort(), error.value(), error.message().c_str());
	}
}

void Session::handleReadAuthRspFromRemote(const boost::system::error_code &error, size_t bytes)
{
	if (!error)
	{
		rsock_->async_read_some(boost::asio::buffer(rbuffer_),
			boost::bind(&Session::handleWriteSockv5RspToBrowser, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	}
	else
	{
		SF_LOG_WITH_ADDRESS(LOG_ERROR, "Write auth data to remote failed, will close!"); 
	}
}


void Session::handleWriteSockv5RspToBrowser(const boost::system::error_code & error, size_t bytes)
{
	do
	{
		if (error) {
			SF_LOG_WITH_ADDRESS(LOG_ERROR, "Read auth rsp from remote svr failed, error:%d, errmsg:%s", error.value(), error.message().c_str());
			break;
		}
		tmpRBuffer_ += std::string(rbuffer_.data(), bytes);
		int32_t len = FrameFacade::get_mutable_instance().check(tmpRBuffer_);
		if (len == 0)
		{
			rsock_->async_read_some(boost::asio::buffer(rbuffer_),
				boost::bind(&Session::handleReadAuthRspFromRemote, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
			return;
		}
		std::string data = tmpRBuffer_.substr(0, len);
		CodecFacade::get_mutable_instance().decode(FrameFacade::get_mutable_instance().remoteFrames(data));
		tmpRBuffer_ = tmpRBuffer_.substr(len);
		ProxyMsgPkg pkg;
		proxy::msg::ProxyConnectRsp rsp;
		int ret = pkg.parseAndCheck(data);
		if(ret != PROXY_PARSE_OK)
		{
			SF_LOG_WITH_ADDRESS(LOG_ERROR, "Parse connect rsp failed, data size:%d, ret:%d, errmsg:%s, checksum:%d", (int)data.size(), ret, pkg.lastErrorMsg().c_str(), pkg.package().check_sum());
            break;
		}

		if (!rsp.ParseFromString(pkg.package().body()))
		{
			SF_LOG_WITH_ADDRESS(LOG_ERROR, "Parse auth rsp data failed, data size:%d, checksum:%d", (int)pkg.package().body().size(), pkg.package().check_sum());
			break;
		}
		if (rsp.result() != 0)
		{
			SF_LOG_WITH_ADDRESS(LOG_ERROR, "Create connect to remote, but got result != 0, result:%d, user:%s, password:%s",
				rsp.result(),
				getUser().c_str(),
				getPassword().c_str()
				);
			break;
		}
		sockReqStringData_[1] = 0x0;
		boost::asio::async_write(*sock_, boost::asio::buffer(sockReqStringData_),
			boost::bind(&Session::handleBeginPipeTransfer, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
		
		return;
	} while (false);
    shutdown();
}

void Session::handleBeginPipeTransfer(const boost::system::error_code &error, size_t bytes)
{
	if (!error)
	{
		SF_LOG_WITH_ADDRESS(LOG_TRACE, "Write sockv5 rsp to browser success, begin pipe data!");
		rsock_->async_read_some(boost::asio::buffer(rbuffer_),
			boost::bind(&Session::handleReadDataFromRemote, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
		sock_->async_read_some(boost::asio::buffer(bbuffer_),
			boost::bind(&Session::handleReadDataFromBrowser, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	}
	else
	{
		SF_LOG_WITH_ADDRESS(LOG_ERROR, "Write sockv5 rsp to browser failed, error:%d, errmsg:%s", error.value(), error.message().c_str());
	}
}

void Session::handleReadDataFromBrowser(const boost::system::error_code & error, size_t bytes)
{
	if (!error)
	{
		tmpBBuffer_ += std::string(bbuffer_.data(), bytes);
		std::string buf;
		ProxyMsgPkg pkg;
		pkg.createDataPkg(tmpBBuffer_).serializedToString(buf);
		SF_LOG_WITH_ADDRESS(LOG_TRACE, "Recv from browser, current size:%d, recv size:%d", (int)tmpBBuffer_.size(), (int)bytes);
		tmpBBuffer_.clear();
        FrameFacade::get_mutable_instance().addFrames(CodecFacade::get_mutable_instance().encode(buf));
		tmpBBuffer_.swap(buf);
		SF_LOG_WITH_ADDRESS(LOG_TRACE, "Begin write data to remote, size:%d", (int)tmpBBuffer_.size());
		boost::asio::async_write(*rsock_, boost::asio::buffer(tmpBBuffer_),
			boost::bind(&Session::handleWriteDataToRemote, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	}
	else
	{
		SF_LOG_WITH_ADDRESS(LOG_ERROR, "Read data from browser failed, error:%d, errmsg:%s", error.value(), error.message().c_str());
		shutdown();
	}
}

void Session::handleReadDataFromRemote(const boost::system::error_code &error, size_t bytes)
{
	if (!error)
	{
		tmpRBuffer_ += std::string(rbuffer_.data(), bytes);
		int32_t msgCount = 0;
		boost::shared_ptr<std::string> allPacket(new std::string());
		while(true) 
		{
			int32_t len = FrameFacade::get_mutable_instance().check(tmpRBuffer_);
			if (len == 0)
			{
				if(msgCount == 0)
				{
					rsock_->async_read_some(boost::asio::buffer(rbuffer_),
						boost::bind(&Session::handleReadDataFromRemote, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
				}
				else
				{
					SF_LOG_WITH_ADDRESS(LOG_TRACE, "Recv %d msg from remote, size:%d, send all to browser", msgCount, (int)allPacket->size());
					auto thisPtr = shared_from_this();
					boost::asio::async_write(*sock_, boost::asio::buffer(*allPacket), [allPacket, thisPtr](const boost::system::error_code &error, size_t bytes)->void {
						thisPtr->handleWriteDataToBrowser(error, bytes);
					});
				}
				return;
			}
			++msgCount;
			std::string data = tmpRBuffer_.substr(0, len);
			tmpRBuffer_ = tmpRBuffer_.substr(len);
			CodecFacade::get_mutable_instance().decode(FrameFacade::get_mutable_instance().remoteFrames(data));
			ProxyMsgPkg pkg;
			int ret = pkg.parseAndCheck(data);
			if (ret != PROXY_PARSE_OK)
			{
				SF_LOG_WITH_ADDRESS(LOG_ERROR, "Paser package failed, data size:%d, ret:%d, errmsg:%s, checksum:%d", (int)data.size(), ret, pkg.lastErrorMsg().c_str(), pkg.package().check_sum());
				shutdown();
				return;
			}

			if (pkg.package().subcmd() != proxy::msg::SubCmd0x2) //data cmd
			{
				SF_LOG_WITH_ADDRESS(LOG_ERROR, "Read unknow subcmd:%d, cmd:%d, data size:%d, skip this msg.",
					pkg.package().subcmd(), pkg.package().cmd(), pkg.package().has_body() ? (int)pkg.package().body().size() : 0);
				continue;
			}
			allPacket->append(pkg.package().body());
		}
	}
	else
	{
		SF_LOG_WITH_ADDRESS(LOG_ERROR, "Read data from remote failed, error:%d, errmsg:%s", error.value(), error.message().c_str());
		shutdown();
	}
}

void Session::handleWriteDataToRemote(const boost::system::error_code & error, size_t bytes)
{
	if (!error)
	{
		tmpBBuffer_.clear();
		sock_->async_read_some(boost::asio::buffer(bbuffer_),
			boost::bind(&Session::handleReadDataFromBrowser, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	}
	else
	{
		SF_LOG_WITH_ADDRESS(LOG_ERROR, "Write data to remote failed, error:%d, errmsg:%s", error.value(), error.message().c_str());
		shutdown();
	}
}

void Session::handleWriteDataToBrowser(const boost::system::error_code &error, size_t bytes)
{
	if (!error)
	{
		rsock_->async_read_some(boost::asio::buffer(rbuffer_),
			boost::bind(&Session::handleReadDataFromRemote, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	}
	else
	{
		SF_LOG_WITH_ADDRESS(LOG_ERROR, "Write data to browser failed, error:%d, errmsg:%s", error.value(), error.message().c_str());
		shutdown();
	}
}

void Session::shutdown()
{
	boost::system::error_code ignored_ec;
	// Զ�̺ͱ������Ӷ����ر�.
	if(sock_->is_open())
	{
		//sock_->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
		sock_->close(ignored_ec);
	}
	if(rsock_->is_open())
	{
		//sock_->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
		rsock_->close(ignored_ec);
	}
}
