set(SRC ../codec/CodecFacade.cpp ../frame/FrameFacade.cpp Logger.cpp Md5.cpp RandomUtils.cpp CRC32.cpp)

add_library(utils STATIC ${SRC})

if(NOT WIN32)
    target_link_libraries(utils pthread libboost_date_time.a libboost_system.a libboost_thread.a)
endif()