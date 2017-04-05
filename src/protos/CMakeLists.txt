set(SRC ProxyMsgPkg.cpp ProxyMsg.pb.cc)

add_library(protos STATIC ${SRC})
target_link_libraries(protos utils)
if(NOT WIN32)
    target_link_libraries(protos libprotobuf.a libboost_system.a)
endif()