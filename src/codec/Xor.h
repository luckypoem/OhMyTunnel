//
// Created by sen on 2017/4/3.
//

#ifndef OHMYTUNNEL_XOR_H
#define OHMYTUNNEL_XOR_H

#include "interface/BaseCodec.h"

class Xor : public BaseCodec
{
public:
    Xor() : BaseCodec("xor") {}

public:
    virtual bool encode(std::string &raw, const std::string &key) override;
    virtual bool decode(std::string &raw, const std::string &key) override;
};


#endif //OHMYTUNNEL_XOR_H
