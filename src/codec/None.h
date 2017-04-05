//
// Created by sen on 2017/4/3.
//

#ifndef OHMYTUNNEL_NONE_H
#define OHMYTUNNEL_NONE_H

#include "interface/BaseCodec.h"

class None : public BaseCodec
{
public:
    None();

    virtual bool encode(std::string &raw, const std::string &key) override;

    virtual bool decode(std::string &raw, const std::string &key) override;

};


#endif //OHMYTUNNEL_NONE_H
