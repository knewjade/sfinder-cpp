cmake_minimum_required(VERSION 3.8)

project(sfinder)

# Load build projects
add_subdirectory(build)

# Load googletest projects if repository is pulled
file(GLOB RESULT googletest/*)
list(LENGTH RESULT RES_LEN)
if (NOT RES_LEN EQUAL 0)
    add_subdirectory(googletest)
endif ()