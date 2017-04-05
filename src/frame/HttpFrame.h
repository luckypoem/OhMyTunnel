//
// Created by sen on 2017/4/3.
//

#ifndef OHMYTUNNEL_HTTPHEADER_H
#define OHMYTUNNEL_HTTPHEADER_H

#include "interface/BaseFrame.h"
#include <map>

class HttpFrame : public BaseFrame
{
public:
    HttpFrame(const std::string host, const std::string &name) : BaseFrame(name), host_(host) { }

public:
    virtual int32_t check(const std::string &data) override;
    virtual int32_t checkGet(const std::string &data);
    virtual int32_t checkPost(const std::string &data);
    virtual int32_t checkRsp(const std::string &data);
    virtual int32_t unknowCheck(const std::string &data) { return -1; }

    virtual std::string &removeFrame(std::string &data);

protected:
    const std::string &getHost() const { return host_; }

private:
    std::string host_;
};


#endif //OHMYTUNNEL_HTTPHEADER_H
