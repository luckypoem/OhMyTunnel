//
// Created by sen on 2017/4/3.
//

#include "Rc4.h"
#include <string.h>

bool Rc4::encode(std::string &raw, const std::string &key)
{
    struct rc4_state rc4State;
    rc4_setup(&rc4State, (unsigned char *)key.c_str(), key.size());  //
    rc4_crypt(&rc4State, (unsigned char *)raw.c_str(), raw.size());
    return true;
}

bool Rc4::decode(std::string &raw, const std::string &key)
{
    return encode(raw, key);
}

void Rc4::rc4_setup( struct rc4_state *s, unsigned char *key,  int length )
{
    int i, j, k, *m, a;

    s->x = 0;
    s->y = 0;
    m = s->m;

    for( i = 0; i < 256; i++ )
    {
        m[i] = i;
    }

    j = k = 0;

    for( i = 0; i < 256; i++ )
    {
        a = m[i];
        j = (unsigned char) ( j + a + key[k] );
        m[i] = m[j]; m[j] = a;
        if( ++k >= length ) k = 0;
    }
}

void Rc4::rc4_crypt( struct rc4_state *s, unsigned char *data, int length )
{
    int i, x, y, *m, a, b;

    x = s->x;
    y = s->y;
    m = s->m;

    for( i = 0; i < length; i++ )
    {
        x = (unsigned char) ( x + 1 ); a = m[x];
        y = (unsigned char) ( y + a );
        m[x] = b = m[y];
        m[y] = a;
        data[i] ^= m[(unsigned char) ( a + b )];
    }

    s->x = x;
    s->y = y;
}