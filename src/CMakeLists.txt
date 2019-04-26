cmake_minimum_required(VERSION 3.6)

set(LIBRARY_TYPE SHARED)
set(PROJECT_NAME sfinder)
set(CMAKE_CXX_STANDARD 17)

project(${PROJECT_NAME})

file(GLOB SRC *.cpp *.hpp core/*.cpp core/*.hpp)

add_definitions(-D${PROJECT_NAME}_EXPORTS)

include_directories(${PROJECT_BINARY_DIR})

add_library(${PROJECT_NAME} ${LIBRARY_TYPE} ${SRC})