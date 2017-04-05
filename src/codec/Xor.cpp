//
// Created by sen on 2017/4/3.
//

#include "Xor.h"

bool Xor::encode(std::string &raw, const std::string &key)
{
    for(int i = 0; i < raw.size(); ++i)
        raw[i] ^= (~0);
    return true;
}

bool Xor::decode(std::string &raw, const std::string &key)
{
    for(int i = 0; i < raw.size(); ++i)
        raw[i] ^= (~0);
    return true;
}
