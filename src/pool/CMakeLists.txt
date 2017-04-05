set(SRC IoServicePool.cpp)

add_library(pool STATIC ${SRC})

target_link_libraries(pool utils)

if(NOT WIN32)
    target_link_libraries(pool pthread libboost_system.a libboost_thread.a)
endif()