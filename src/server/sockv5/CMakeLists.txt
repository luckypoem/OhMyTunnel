set(SRC Session.cpp Sockv5Proxy.cpp Setting.cpp)

add_library(sockv5 STATIC ${SRC})

target_link_libraries(sockv5  utils pool proxy_interface)

if(NOT WIN32)
    target_link_libraries(sockv5 libboost_system.a)
endif()