#include "CodecFacade.h"
#include "codec/Chacha20.h"
#include "codec/None.h"
#include "codec/Rc4.h"
#include "codec/Reverse.h"
#include "codec/Xor.h"
#include <map>
#include "utils/Logger.h"

CodecFacade::CodecFacade()
{
}


CodecFacade::~CodecFacade()
{
}

std::string &CodecFacade::encode(std::string & raw)
{
	for(auto &codec : codecList_)
	{
		//uint32_t before = raw.size();
		codec->encode(raw, key_);
		//SF_LOG(LOG_TRACE, "Using codec:%s, before:%d, after:%d, data:%s", codec->getName().c_str(), before, (int)raw.size(), raw.c_str());
	}
	return raw;
}

std::string &CodecFacade::decode(std::string & raw)
{
	for(CodecList::const_reverse_iterator it = codecList_.rbegin(); it != codecList_.rend(); ++it)
	{
		//uint32_t before = raw.size();
		(*it)->decode(raw, key_);
		//SF_LOG(LOG_TRACE, "Using codec:%s, before:%d, after:%d", (*it)->getName().c_str(), before, (int)raw.size());
	}
	return raw;
}

void CodecFacade::init(const std::string &key)
{
	key_ = key;
	std::vector<CodecPtr> vec;

	//You codec here
	vec.push_back(CodecPtr(new Xor()));
	vec.push_back(CodecPtr(new Chacha20()));
	vec.push_back(CodecPtr(new Reverse()));
	vec.push_back(CodecPtr(new None()));
	vec.push_back(CodecPtr(new Rc4()));

	//
	for (auto &item : vec)
	{
		allCodec_.insert(std::make_pair(item->getName(), item));
	}

}

bool CodecFacade::addCodec(CodecPtr codec)
{
	if (codec.get() == nullptr)
		return false;
	codecList_.push_back(codec);
	return true;
}

bool CodecFacade::addCodec(const std::string &name)
{
	auto iter = allCodec_.find(name);
	if (iter == allCodec_.end())
		return false;
	return addCodec(iter->second);
}

std::string CodecFacade::toString() const
{
	std::string codecs = "Codec:[";
	for (auto &item : allCodec_)
	{
		codecs += item.first + ",";
	}
	if(!allCodec_.empty())
		codecs.pop_back();
	codecs.push_back(']');
	return codecs;
}


