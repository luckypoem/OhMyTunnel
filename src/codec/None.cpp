//
// Created by sen on 2017/4/3.
//

#include "None.h"

bool None::encode(std::string &raw, const std::string &key)
{
    return true;
}

bool None::decode(std::string &raw, const std::string &key)
{
    return true;
}

None::None() : BaseCodec("none")
{

}
