//
// Created by sen on 2017/4/3.
//

#include "BaseCodec.h"

BaseCodec::BaseCodec(const std::string &name)
    : name_(name)
{

}

const std::string &BaseCodec::getName() const
{
    return name_;
}
