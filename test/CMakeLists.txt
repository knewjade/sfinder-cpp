cmake_minimum_required(VERSION 3.6)

set(PROJECT_NAME test)
set(SRC_PROJECT_NAME sfinder)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-O3 -Wall")

file(GLOB SRC ${CMAKE_CURRENT_LIST_DIR}/test*.cpp ${CMAKE_CURRENT_LIST_DIR}/*.hpp)

include_directories(${googletest_SOURCE_DIR}/include)
include_directories(${googlemock_SOURCE_DIR}/include)
include_directories(${${SRC_PROJECT_NAME}_SOURCE_DIR})

add_executable(${PROJECT_NAME} ${SRC})

target_link_libraries(${PROJECT_NAME}
        gtest
        gtest_main
        gmock
        gmock_main
        ${SRC_PROJECT_NAME}
        )
