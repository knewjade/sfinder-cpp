cmake_minimum_required(VERSION 3.8)

project(sfinder)

# Check  if googletest is pulled
file(GLOB RESULT googletest/*)
list(LENGTH RESULT RES_LEN)
if (NOT RES_LEN EQUAL 0)
    set(TEST_ENABLE 1)
else ()
    set(TEST_ENABLE 0)
endif ()

# Load build projects
add_subdirectory(build)

# Load googletest projects
if (${TEST_ENABLE} EQUAL 1)
    add_subdirectory(googletest)
endif ()
