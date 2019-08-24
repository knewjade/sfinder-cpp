add_subdirectory (core)

if (${TEST_ENABLE} EQUAL 1)
    add_subdirectory(test)
endif ()