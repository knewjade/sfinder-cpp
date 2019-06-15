cmake_minimum_required (VERSION 3.8)

include(../../vars/sfinder_core.cmake)

project(sfinder_core_mac)

# Compiler options
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-m64 -Wall")
#set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")  # -march=native -s
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -flto -march=native -DNDEBUG")

# Build defines
include_directories(${PROJECT_BINARY_DIR})

add_library(${PROJECT_NAME} SHARED ${sfinder_core_SRC})