//
// Created by sen on 2017/4/3.
//

#ifndef OHMYTUNNEL_RC4_H
#define OHMYTUNNEL_RC4_H

#include "interface/BaseCodec.h"

/*
 *  From https://github.com/anthonywei/rc4
 *
 * */

class Rc4 : public BaseCodec
{
public:
    Rc4() : BaseCodec("rc4") {}

    virtual bool encode(std::string &raw, const std::string &key) override;
    virtual bool decode(std::string &raw, const std::string &key) override;

private:
    struct rc4_state
    {
        int x, y, m[256];
    };

    void rc4_setup( struct rc4_state *s, unsigned char *key,  int length );
    void rc4_crypt( struct rc4_state *s, unsigned char *data, int length );


};


#endif //OHMYTUNNEL_RC4_H
