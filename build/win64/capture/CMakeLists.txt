cmake_minimum_required (VERSION 3.8)

include(../../vars/sfinder_core.cmake)
include(../../vars/sfinder_capture.cmake)

project(sfinder_capture_win64)

# Compiler options
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-static -lstdc++ -lgcc -lwinpthread -m64 -Wall")
#set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")  # -march=native -s
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -flto -march=native -DNDEBUG")

# Build defines
include_directories(${sfinder_core_SOURCE_DIR})

add_library(${PROJECT_NAME} SHARED ${sfinder_capture_SRC})

target_link_libraries(${PROJECT_NAME}
        sfinder_core_win64
        )
