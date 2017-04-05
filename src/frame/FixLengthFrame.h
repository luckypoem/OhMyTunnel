//
// Created by sen on 2017/4/3.
//

#ifndef OHMYTUNNEL_FIXLENGTHHEADER_H
#define OHMYTUNNEL_FIXLENGTHHEADER_H

#include "interface/BaseFrame.h"

class FixLengthFrame : public BaseFrame
{
public:
    FixLengthFrame() : BaseFrame("fix")
    {}

public:
    virtual int32_t check(const std::string &data) override;

    virtual std::string &removeFrame(std::string &data) override;

    virtual std::string &addFrame(std::string &data) override;

};


#endif //OHMYTUNNEL_FIXLENGTHHEADER_H
