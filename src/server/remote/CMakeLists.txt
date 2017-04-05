set(SRC RemoteSvr.cpp RemoteSession.cpp Setting.cpp)

add_library(remotesvr STATIC ${SRC})

target_link_libraries(remotesvr utils pool proxy_interface)

if(NOT WIN32)
    target_link_libraries(remotesvr libboost_system.a)
endif()

