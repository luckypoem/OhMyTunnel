#pragma once

#include <boost/asio.hpp>
#include <vector>
#include <boost/atomic/atomic.hpp>
#include <boost/thread.hpp>

class IoServicePool
{
public:
	IoServicePool(uint32_t count = 4);
	~IoServicePool();

	void start();

public:
	boost::shared_ptr<boost::asio::io_service> get();
	boost::shared_ptr<boost::asio::io_service> get(uint32_t index);

private:
	std::vector<boost::shared_ptr<boost::asio::io_service>> vec_;
	std::vector<boost::shared_ptr<boost::asio::io_service::work>> wait_;
	boost::atomic<uint32_t> nextIndex_;
	std::vector<boost::shared_ptr<boost::thread>> threadPool_;
	bool start_;
	int32_t count_;
};

