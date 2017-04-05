//
// Created by sen on 16-5-15.
//

#ifndef SOCKSTUNNELLOCAL_RANDOMUTILS_H
#define SOCKSTUNNELLOCAL_RANDOMUTILS_H

#include <string>
#include <stdlib.h>

typedef std::string String;

class RandomUtils
{
public:
    static void setSeed(unsigned int seed);
    static int randInt();
    static int randInt(int max);
    static int randInt(int min, int max);
    static bool randBool();
    static char randChar();
    static String randString(int length);
    static String randStringVisible(int length);
};


#endif //SOCKSTUNNELLOCAL_RANDOMUTILS_H
