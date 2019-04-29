cmake_minimum_required(VERSION 3.6)

set(LIBRARY_TYPE SHARED)
set(PROJECT_NAME sfinder)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall")
#set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")  # -march=native -s
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -flto -march=native -DNDEBUG")

project(${PROJECT_NAME})

file(GLOB SRC *.cpp *.hpp core/*.cpp core/*.hpp finder/*.cpp finder/*.hpp)

add_definitions(-D${PROJECT_NAME}_EXPORTS)

include_directories(${PROJECT_BINARY_DIR})

add_library(${PROJECT_NAME} ${LIBRARY_TYPE} ${SRC})