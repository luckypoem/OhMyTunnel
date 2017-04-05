//
// Created by sen on 2017/4/3.
//

#include "Chacha20.h"

bool Chacha20::encode(std::string &raw, const std::string &key)
{
    uint8_t realKey[32] = {0};
    uint8_t nonce[8] = {0};
    memcpy((char *)realKey, key.c_str(), sizeof(realKey));
    memcpy((char *)nonce, key.c_str(), sizeof(nonce));
    Chacha20Codec codec(realKey, nonce, key.size());
    codec.crypt((uint8_t *)raw.c_str(), raw.size());
    return true;
}

bool Chacha20::decode(std::string &raw, const std::string &key)
{
    return encode(raw, key);
}
