//
// Created by sen on 2017/4/3.
//

#ifndef OHMYTUNNEL_BASECODEC_H
#define OHMYTUNNEL_BASECODEC_H

#include <string>

class BaseCodec
{
public:
    BaseCodec(const std::string &name);
    virtual ~BaseCodec() {}

public:
    virtual bool init() { return true; }
    virtual bool encode(std::string &raw, const std::string &key) = 0;
    virtual bool decode(std::string &raw, const std::string &key) = 0;
    virtual void destroy() { }

public:
    const std::string &getName() const;

private:
    std::string name_;
};


#endif //OHMYTUNNEL_BASECODEC_H
