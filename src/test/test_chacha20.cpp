//
// Created by sen on 2017/4/3.
//
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE CHACHA20
#include <boost/test/unit_test.hpp>
#include "codec/Chacha20.h"
#include <iostream>

BOOST_AUTO_TEST_SUITE(test_chacha20)

    BOOST_AUTO_TEST_CASE( test_encode_decode )
    {
        Chacha20 chacha20;
        std::string raw = "hello world";
        std::string backup = raw.substr(0);
        std::string key = "1234567";
        chacha20.encode(raw, key);

        Chacha20 &chacha20_2 = chacha20;
        chacha20_2.decode(raw, key);
        std::cout << "Back up:" << backup << " " << "Raw:" << raw << std::endl;
        std::cout << "Equ=" << (raw == backup) << std::endl;
        BOOST_WARN(raw == backup);
    }

BOOST_AUTO_TEST_SUITE_END()