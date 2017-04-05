set(SRC ../interface/BaseFrame.cpp FixLengthFrame.cpp HttpFrame.cpp HttpClientFrame.cpp HttpClientFrame.h HttpServerFrame.cpp HttpServerFrame.h)

add_library(proxy_frame STATIC ${SRC})

if(NOT WIN32)
    target_link_libraries(proxy_frame proxy_interface libboost_system.a)
endif()