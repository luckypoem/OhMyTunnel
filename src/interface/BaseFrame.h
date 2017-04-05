//
// Created by sen on 2017/4/3.
//

#ifndef OHMYTUNNEL_BASEHEADER_H
#define OHMYTUNNEL_BASEHEADER_H

#include <string>
#include <stdint.h>

class BaseFrame
{
public:
    BaseFrame(const std::string &name);
    virtual ~BaseFrame() {}

public:
    /*
     *  len < 0 package err
     *  len == 0 need more data
     *  len >0 recv end
     *
     * */
    virtual int32_t check(const std::string &data) = 0;
    virtual std::string &addFrame(std::string &data) = 0;
    virtual std::string &removeFrame(std::string &data) = 0;

public:
    const std::string &getName() const { return name_; }

private:
    std::string name_;
};


#endif //OHMYTUNNEL_BASEHEADER_H
