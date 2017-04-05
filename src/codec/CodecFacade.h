#pragma once

#include <string>
#include <boost/serialization/singleton.hpp>
#include <boost/shared_ptr.hpp>
#include "interface/BaseCodec.h"
#include <vector>
#include <map>

class CodecFacade : public boost::serialization::singleton<CodecFacade>
{
	typedef boost::shared_ptr<BaseCodec> CodecPtr;
	typedef std::vector<CodecPtr> CodecList;
	typedef std::map<std::string, CodecPtr> CodecMap;

public:
	CodecFacade();
	~CodecFacade();

public:
	void init(const std::string &key);
	bool addCodec(CodecPtr codec);
	bool addCodec(const std::string &name);

public:
	std::string &encode(std::string &raw);
	std::string &decode(std::string &raw);

public:
	std::string toString() const;

private:
	std::string key_;
	CodecList codecList_;
	CodecMap allCodec_;
};

