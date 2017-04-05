#include "IoServicePool.h"
#include "utils/Logger.h"


IoServicePool::IoServicePool(uint32_t count)
	: nextIndex_(0), start_(false), count_(count)
{

}


IoServicePool::~IoServicePool()
{
}

void IoServicePool::start()
{
	if (start_)
		return;
	for (int i = 0; i < count_; ++i)
	{
		boost::shared_ptr<boost::asio::io_service> io(new boost::asio::io_service());
		vec_.push_back(io);
		boost::shared_ptr<boost::asio::io_service::work> work(new boost::asio::io_service::work(*io));
		wait_.push_back(work);
		boost::shared_ptr<boost::thread> thr(new boost::thread([io, i]()->void {
			SF_LOG(LOG_INFO, "Ioservice:%d start up!", i);
			io->run();
		}));
		threadPool_.push_back(thr);
	}
	start_ = true;
}

boost::shared_ptr<boost::asio::io_service> IoServicePool::get()
{
	return get(nextIndex_++);
}

boost::shared_ptr<boost::asio::io_service> IoServicePool::get(uint32_t index)
{
	return vec_.at(index % vec_.size());
}
