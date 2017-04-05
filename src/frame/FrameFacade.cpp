#include "FrameFacade.h"
#include <boost/asio.hpp>
#include "frame/FixLengthFrame.h"
#include "frame/HttpClientFrame.h"
#include "frame/HttpServerFrame.h"

FrameFacade::FrameFacade()
{
}


FrameFacade::~FrameFacade()
{
}

std::string & FrameFacade::addFrames(std::string &buf)
{
    for(auto &header : headerList_)
        header->addFrame(buf);
    return buf;
//	uint32_t len = buf.size();
//	len = boost::asio::detail::socket_ops::host_to_network_long(len);
//	std::string tmp((const char *)&len, 4);
//	tmp.append(buf);
//	tmp.swap(buf);
//	return buf;
}

std::string & FrameFacade::remoteFrames(std::string &buf)
{
    for(HeaderList::const_reverse_iterator it = headerList_.rbegin(); it != headerList_.rend(); ++it)
    {
        (*it)->removeFrame(buf);
    }
    return buf;
//	uint32_t len = *reinterpret_cast<const uint32_t *>(buf.c_str());
//	len = boost::asio::detail::socket_ops::network_to_host_long(len);
//	buf.substr(4, len).swap(buf);
//	return buf;
}
/*
 *  header1->header2->...->headern, should use header n to check pkg.
 * */
int32_t FrameFacade::check(const std::string &data)
{
    return headerList_.back()->check(data);
//	uint32_t len = *reinterpret_cast<const uint32_t *>(data.c_str());
//	len = boost::asio::detail::socket_ops::network_to_host_long(len);
//	if (len + 4 > data.size())
//		return 0;
//	return len + 4;
}

bool FrameFacade::addFrame(FrameFacade::HeaderPtr header)
{
	if(header.get() == nullptr)
		return false;
	headerList_.push_back(header);
	return true;
}

std::string FrameFacade::toString() const
{
	std::string headers = "Frame:[";

	for (auto &header : headers_)
		headers += header.first + ",";

	if(!headers_.empty())
		headers.pop_back();
	headers += "]";
	return headers;
}

bool FrameFacade::addFrame(const std::string &header)
{
    auto iter = headers_.find(header);
    if(iter == headers_.end())
        return false;
    return addFrame(iter->second);
}

void FrameFacade::init()
{
    HeaderList vec;
    //Add you header here
    vec.push_back(HeaderPtr(new HttpClientFrame("")));
    vec.push_back(HeaderPtr(new HttpServerFrame("")));
    vec.push_back(HeaderPtr(new FixLengthFrame()));

    //
    for(auto &header : vec)
    {
        headers_.insert(std::make_pair(header->getName(), header));
    }
}
