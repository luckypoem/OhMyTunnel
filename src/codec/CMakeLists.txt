set(SRC ../interface/BaseCodec.cpp Xor.cpp Rc4.cpp Chacha20 None.cpp None.h Reverse.cpp Reverse.h)

add_library(codec STATIC ${SRC})

if(NOT WIN32)
    target_link_libraries(codec proxy_interface libboost_system.a)
endif()