#pragma once

#include <boost/serialization/singleton.hpp>
#include <string>
#include "interface/BaseFrame.h"
#include <vector>
#include <boost/shared_ptr.hpp>
#include <map>

class FrameFacade : public boost::serialization::singleton<FrameFacade>
{
	typedef boost::shared_ptr<BaseFrame> HeaderPtr;
	typedef std::vector<HeaderPtr> HeaderList;
	typedef std::map<std::string, HeaderPtr> HeaderMap;

public:
	FrameFacade();
	~FrameFacade();

public:
    void init();
	bool addFrame(HeaderPtr header);
    bool addFrame(const std::string &header);

public:
	std::string &addFrames(std::string &buf);
	std::string &remoteFrames(std::string &buf);
	int32_t check(const std::string &data);

public:
    std::string toString() const;

private:
	HeaderList headerList_;
	HeaderMap headers_;
};

