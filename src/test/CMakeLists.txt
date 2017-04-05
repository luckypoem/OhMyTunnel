set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/test_bin)

set(SRC test_chacha20.cpp)

set(BASE_LIB boost_unit_test_framework)

if(NOT WIN32)
    add_executable(test_chacha20 ${SRC})
    target_link_libraries(test_chacha20 codec ${BASE_LIB})
endif()

