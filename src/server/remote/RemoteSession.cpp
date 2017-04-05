#include "RemoteSession.h"
#include "utils/SessionLogger.h"
#include "frame/FrameFacade.h"
#include "codec/CodecFacade.h"
#include "protos/ProxyMsgPkg.h"
#include <string>
#include <boost/lexical_cast.hpp>

RemoteSession::RemoteSession(boost::shared_ptr<boost::asio::ip::tcp::socket> sock)
	: ProxySession("remote"), sock_(sock),
	rsock_(new boost::asio::ip::tcp::socket(sock->get_io_service())), 
	resolver_(new boost::asio::ip::tcp::resolver(sock->get_io_service()))
{
	
}


RemoteSession::~RemoteSession()
{
	SF_LOG(LOG_TRACE, "Remote session destroy, address:%s, port:%d", getAddress().c_str(), getPort());
}

void RemoteSession::start()
{
	sock_->async_read_some(boost::asio::buffer(bbuffer_),
		boost::bind(&RemoteSession::handleAuthFromBrowser, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void RemoteSession::handleAuthFromBrowser(const boost::system::error_code & error, size_t bytes)
{
	do
	{
		if (error) {
			SF_LOG_WITH_CLIENT(LOG_ERROR, "Read auth data from browser failed!");
			break;
		}
		tmpBBuffer_ += std::string(bbuffer_.data(), bytes);
		int32_t len = FrameFacade::get_mutable_instance().check(tmpBBuffer_);
		if (len == 0)
		{
			sock_->async_read_some(boost::asio::buffer(bbuffer_),
				boost::bind(&RemoteSession::handleAuthFromBrowser, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
			return;
		}
		std::string data = tmpBBuffer_.substr(0, len);
		tmpBBuffer_ = tmpBBuffer_.substr(len);
		CodecFacade::get_mutable_instance().decode(FrameFacade::get_mutable_instance().remoteFrames(data));
		ProxyMsgPkg &pkg = authPkg_;
		proxy::msg::ProxyConnectReq req;
		int32_t ret = pkg.parseAndCheck(data);
		if(ret != PROXY_PARSE_OK)
		{
			SF_LOG_WITH_ADDRESS(LOG_ERROR, "Parse pb package failed, ret:%d, errmsg:%s, data size:%d", ret, pkg.lastErrorMsg().c_str(), (int)data.size());
			break;
		}
		if (!req.ParseFromString(pkg.package().body()))
		{
			SF_LOG_WITH_CLIENT(LOG_ERROR, "Connect data parse failed, data len:%d, checksum:%d, close!", (int)data.size(), pkg.package().check_sum());
			break;
		}
		if (!checkUser(req.username(), req.password()))
		{
			SF_LOG_WITH_CLIENT(LOG_ERROR, "Check user failed, user:%s, pwd:%s", req.username().c_str(), req.password().c_str());
			break;
		}
		if (req.type() != 0x1 && req.type() != 0x3 && req.type() != 0x4) //
		{
			SF_LOG_WITH_CLIENT(LOG_ERROR, "Client send an invalid address type:%d, user:%s", req.type(), req.username().c_str());
			return;
		}

		//for log
		setAddress(req.type() != 0x3 ? boost::asio::ip::address().from_string(req.address()).to_string() : req.address());
		setPort(req.port());
		setUser(req.username());
		//

		SF_LOG_WITH_CLIENT(LOG_INFO, "Recv connection, user:%s, type:%d, address:%s, port:%d", req.username().c_str(), req.type(), getAddress().c_str(), getPort());
		if (req.type() == 0x3)
		{
			boost::asio::ip::tcp::resolver::query qry(getAddress(), boost::lexical_cast<std::string>(getPort()));
			resolver_->async_resolve(qry, boost::bind(
				&RemoteSession::handleQueryDns, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::iterator));
			return;
		}
		else
		{
			rsock_->async_connect(boost::asio::ip::tcp::endpoint(boost::asio::ip::address().from_string(getAddress()), getPort()),
				boost::bind(&RemoteSession::handleConnectToTarget, shared_from_this(), boost::asio::placeholders::error));
			return;
		}
	} while (false);
	shutdown();
}

void RemoteSession::handleQueryDns(const boost::system::error_code &error, boost::asio::ip::tcp::resolver::iterator ep)
{
	if (!error)
	{
		SF_LOG_WITH_ADDRESS(LOG_INFO, "Query dns success, ip:%s, port:%d", ep->endpoint().address().to_string().c_str(), (int)ep->endpoint().port());
		boost::asio::async_connect(*rsock_, ep, 
			boost::bind(&RemoteSession::handleConnectToTarget, shared_from_this(), boost::asio::placeholders::error));
	}
	else
	{
		SF_LOG_WITH_ADDRESS(LOG_ERROR, "Query dns failed, error:%d, errmsg:%s", error.value(), error.message().c_str());
	}
}

void RemoteSession::handleConnectToTarget(const boost::system::error_code & error)
{
	if (!error)
	{
		rsock_->set_option(boost::asio::ip::tcp::acceptor::linger(true, 0));
		SF_LOG_WITH_ADDRESS(LOG_INFO, "Connect to remote success, begin write rsp to browser!");
		boost::shared_ptr<std::string> data(new std::string());
		auto thisPtr = shared_from_this();
		ProxyMsgPkg pkg;
		proxy::msg::ProxyConnectRsp rsp;
		rsp.set_result(0);
		rsp.SerializeToString(&(*data));
		pkg.createCommonRsp(authPkg_, *data, 0);
		pkg.serializedToString(*data);
        FrameFacade::get_mutable_instance().addFrames(CodecFacade::get_mutable_instance().encode(*data));
		boost::asio::async_write(*sock_, boost::asio::buffer(*data), [thisPtr, data](const boost::system::error_code & error, size_t bytes)->void {
			thisPtr->handleWriteRespToBrowser(error, bytes);
		});
	}
	else
	{
		SF_LOG_WITH_ADDRESS(LOG_ERROR, "Connect to remote failed!");
	}
}

void RemoteSession::handleWriteRespToBrowser(const boost::system::error_code & error, size_t bytes)
{
	if (!error)
	{
		SF_LOG_WITH_ADDRESS(LOG_INFO, "Write rsp to browser success, begin pipe data.");
		sock_->async_read_some(boost::asio::buffer(bbuffer_),
			boost::bind(&RemoteSession::handleWriteDataToTarget, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
		rsock_->async_read_some(boost::asio::buffer(rbuffer_), 
			boost::bind(&RemoteSession::handleWriteDataToBrowser, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	}
	else
	{
		SF_LOG_WITH_ADDRESS(LOG_ERROR, "Write rsp to browser failed, skip pipe data.");
	}
}

void RemoteSession::handleReadDataFromTarget(const boost::system::error_code & error, size_t bytes)
{
	if (!error)
	{
		rsock_->async_read_some(boost::asio::buffer(rbuffer_),
			boost::bind(&RemoteSession::handleWriteDataToBrowser, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	}
	else
	{
		SF_LOG_WITH_ADDRESS(LOG_ERROR, "Write data to browser failed, error:%d, errmsg:%s", error.value(), error.message().c_str());
		shutdown();
	}
}

void RemoteSession::handleReadDataFromBrowser(const boost::system::error_code & error, size_t bytes)
{
	if (!error)
	{
		sock_->async_read_some(boost::asio::buffer(bbuffer_),
			boost::bind(&RemoteSession::handleWriteDataToTarget, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	}
	else
	{
		SF_LOG_WITH_ADDRESS(LOG_ERROR, "Write data to remote failed, error:%d, errmsg:%s", error.value(), error.message().c_str());
		shutdown();
	}
}

void RemoteSession::handleWriteDataToBrowser(const boost::system::error_code & error, size_t bytes)
{
	if (!error)
	{
		tmpRBuffer_ += std::string(rbuffer_.data(), bytes);
		ProxyMsgPkg pkg;
		boost::shared_ptr<std::string> tmp(new std::string());
		pkg.createDataPkg(tmpRBuffer_).serializedToString(*tmp);
        FrameFacade::get_mutable_instance().addFrames(CodecFacade::get_mutable_instance().encode(*tmp));
		SF_LOG_WITH_ADDRESS(LOG_TRACE, "Recv %d data from remote, create pkg, pkg size:%d", (int)tmpRBuffer_.size(), (int)tmp->size());
		tmpRBuffer_.clear();
		auto thisPtr = shared_from_this();
		boost::asio::async_write(*sock_, boost::asio::buffer(*tmp), 
			[thisPtr, tmp](const boost::system::error_code & error, size_t bytes) -> void {
				thisPtr->handleReadDataFromTarget(error, bytes);
		});
	}
	else
	{
		SF_LOG_WITH_ADDRESS(LOG_ERROR, "Recv data from remote failed, error:%d, errmsg:%s", error.value(), error.message().c_str());
		shutdown();
	}
}

void RemoteSession::handleWriteDataToTarget(const boost::system::error_code & error, size_t bytes)
{
	if (!error)
	{
		tmpBBuffer_ += std::string(bbuffer_.data(), bytes);
		int32_t msgCount = 0;
		boost::shared_ptr<std::string> allPacket(new std::string());
		while (true)
		{
			int32_t len = FrameFacade::get_mutable_instance().check(tmpBBuffer_);
			if (len == 0)
			{
				if(msgCount == 0)
				{
					sock_->async_read_some(boost::asio::buffer(bbuffer_),
						boost::bind(&RemoteSession::handleWriteDataToTarget, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
				}
				else
				{
					SF_LOG_WITH_ADDRESS(LOG_TRACE, "Recv %d msg from remote, size:%d, send all to target", msgCount, (int)allPacket->size());
					auto thisPtr = shared_from_this();
					boost::asio::async_write(*rsock_, boost::asio::buffer(*allPacket), [thisPtr, allPacket](const boost::system::error_code & error, size_t bytes)->void {
						thisPtr->handleReadDataFromBrowser(error, bytes);
					});
				}
				return;
			}
			++msgCount;
			std::string data(tmpBBuffer_.substr(0, len));
			tmpBBuffer_ = tmpBBuffer_.substr(len);
			auto thisPtr = shared_from_this();
			CodecFacade::get_mutable_instance().decode(FrameFacade::get_mutable_instance().remoteFrames(data));

			ProxyMsgPkg pkg;
			int32_t ret = pkg.parseAndCheck(data);
			if (ret != PROXY_PARSE_OK)
			{
				SF_LOG_WITH_ADDRESS(LOG_ERROR, "Parse browser data failed, data size:%d, ret:%d, errmsg:%s, close it!", (int)data.size(), ret, pkg.lastErrorMsg().c_str());
				shutdown();
				return;
			}
			allPacket->append(pkg.package().body());
		}

	}
	else
	{
		SF_LOG_WITH_ADDRESS(LOG_ERROR, "Recv data from browser failed, error:%d, errmsg:%s", error.value(), error.message().c_str());
		shutdown();
	}
}

bool RemoteSession::checkUser(const std::string &user, const std::string &pwd)
{
	auto iter = userList_.find(user);
	if (iter == userList_.end())
		return false;
	return iter->second == pwd;
}

void RemoteSession::shutdown()
{
	boost::system::error_code ignored_ec;
	if (sock_->is_open())
	{
		//sock_->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
		sock_->close(ignored_ec);
	}
	if (rsock_->is_open())
	{
		//rsock_->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
		rsock_->close(ignored_ec);
	}
}
