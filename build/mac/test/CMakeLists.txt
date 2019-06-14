cmake_minimum_required (VERSION 3.8)

include(../../vars/sfinder_core.cmake)
include(../../vars/sfinder_test.cmake)

project(sfinder_test_mac)

# Compiler options
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-m64 -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")  # -march=native -s
#set(CMAKE_CXX_FLAGS_RELEASE "-O3 -flto -march=native -DNDEBUG")

# Build defines
include_directories(${googletest_SOURCE_DIR}/include)
include_directories(${googlemock_SOURCE_DIR}/include)
include_directories(${sfinder_core_SOURCE_DIR})

add_executable(${PROJECT_NAME} ${sfinder_test_SRC})

target_link_libraries(${PROJECT_NAME}
        gtest
        gtest_main
        gmock
        gmock_main
        sfinder_core_mac
        )
