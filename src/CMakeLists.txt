cmake_minimum_required(VERSION 2.8)
project(OhMyTunnel)

add_definitions("-std=c++11 -g -DBOOST_LOG_DYN_LINK")

if(WIN32)
    include_directories("C:\\Boost\\include/boost-1_60")
    link_directories("C:\\Boost\\lib")
    include_directories("C:\\include")
    link_directories("C:\\lib")
endif(WIN32)

include_directories(${PROJECT_SOURCE_DIR})

add_subdirectory(server)
add_subdirectory(protos)
add_subdirectory(protocol)
add_subdirectory(pool)
add_subdirectory(utils)
add_subdirectory(codec)
add_subdirectory(frame)
add_subdirectory(test)
add_subdirectory(interface)

set(COMMON_SRC )
set(COMMON_LIB protos pool proxy_protocol utils codec proxy_frame proxy_interface)
if(NOT WIN32)
    list(APPEND COMMON_LIB pthread libprotobuf.a libboost_system.a libboost_date_time.a libboost_thread.a libboost_program_options.a)
endif(NOT WIN32)

message("COMMON LIB:${COMMON_LIB}")

set(LOCAL_SRC local.cpp ${COMMON_SRC})
set(REMOTE_SRC remote.cpp ${COMMON_SRC})

add_executable(tun_remote ${REMOTE_SRC})
add_executable(tun_local ${LOCAL_SRC})

target_link_libraries(tun_remote remotesvr ${COMMON_LIB})
target_link_libraries(tun_local sockv5 ${COMMON_LIB})