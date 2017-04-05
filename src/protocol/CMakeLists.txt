set(SRC Sockv5Def.cpp)

add_library(proxy_protocol STATIC ${SRC})

if(NOT WIN32)
    target_link_libraries(proxy_protocol libboost_system.a)
endif()