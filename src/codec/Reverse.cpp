//
// Created by sen on 2017/4/3.
//

#include "Reverse.h"
#include <algorithm>

bool Reverse::encode(std::string &raw, const std::string &key)
{
    int32_t size = getSegSize(raw, key);
    if(size == 0 || size == 1)
        return true;

    int begin = 0;
    int index = size;
    while(true)
    {
        if(index > (signed)raw.size())
            index = raw.size();
        std::reverse(raw.begin() + begin, raw.begin() + index);
        begin = index;
        index += size;
        if(begin == raw.size())
            break;
    }
    return true;
}

bool Reverse::decode(std::string &raw, const std::string &key)
{
    return encode(raw, key);
}

uint32_t Reverse::getSegSize(const std::string &raw, const std::string &key) const
{
    uint32_t hash;
    unsigned char *ptr;

    for(hash = 0, ptr = (unsigned char *)key.c_str(); *ptr ; ptr++)
        hash = 31 * hash + *ptr;
    hash = hash % MAX_SEG_SIZE;
    if(hash == 0)
        hash = MAX_SEG_SIZE;
    return hash;
}
