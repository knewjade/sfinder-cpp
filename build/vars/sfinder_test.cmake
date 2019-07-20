# File defines
set(sfinder_test_SOURCE_DIR ${CMAKE_SOURCE_DIR}/test)

file(GLOB sfinder_test_SRC
        ${sfinder_test_SOURCE_DIR}/main.cpp
        ${sfinder_test_SOURCE_DIR}/*/*.cpp
        ${sfinder_test_SOURCE_DIR}/*/*/*.cpp
        )