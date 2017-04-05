//
// Created by sen on 2017/4/3.
//

#ifndef OHMYTUNNEL_REVERSE_H
#define OHMYTUNNEL_REVERSE_H

#include "interface/BaseCodec.h"
#include <stdint.h>

class Reverse : public BaseCodec
{
    const static int MAX_SEG_SIZE = 512;
public:
    Reverse() : BaseCodec("reverse") {}

public:
    virtual bool encode(std::string &raw, const std::string &key) override;
    virtual bool decode(std::string &raw, const std::string &key) override;

protected:
    uint32_t getSegSize(const std::string &raw, const std::string &key) const;
};


#endif //OHMYTUNNEL_REVERSE_H
