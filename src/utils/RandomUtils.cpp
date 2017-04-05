//
// Created by sen on 16-5-15.
//

#include "RandomUtils.h"

void RandomUtils::setSeed(unsigned int seed)
{
    srand(seed);
}

int RandomUtils::randInt()
{
    return static_cast<int>(rand());
}

int RandomUtils::randInt(int max)
{
    if(max == 0)
        return 0;
    return randInt() % max;
}

int RandomUtils::randInt(int min, int max)
{
    if(min > max)
        std::swap(min, max);
    return min + randInt(max - min);
}

bool RandomUtils::randBool()
{
    return randInt() % 2 == 0;
}

char RandomUtils::randChar()
{
    return static_cast<char>(randInt() % 256);
}

String RandomUtils::randString(int length)
{
    String ret;
    for(auto i = 0; i < length; ++i)
        ret.push_back(randChar());
    return ret;
}

String RandomUtils::randStringVisible(int length)
{
    static String visible = "abcdefghijklnmopqrstuvwxyzABCDEFGHIJKLNMOPQRSTUVWXYZ1234567890";
    String ret;
    for(int i = 0; i < length; ++i)
        ret += visible.at((unsigned int) RandomUtils::randInt(visible.size()));
    return ret;
}
