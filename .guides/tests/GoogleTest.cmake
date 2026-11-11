find_path(GTEST_SOURCE_DIR src/gtest-all.cc
    HINTS /usr/src/googletest/googletest /usr/src/gtest
    DOC "Google Test source directory containing include/ and src/")
if(NOT EXISTS "${GTEST_SOURCE_DIR}/include/gtest/gtest.h")
    message(FATAL_ERROR "Google Test sources are missing. Install libgtest-dev or set -DGTEST_SOURCE_DIR=<path>.")
endif()

find_package(Threads REQUIRED)
add_library(shipwreck_gtest STATIC "${GTEST_SOURCE_DIR}/src/gtest-all.cc")
target_include_directories(shipwreck_gtest SYSTEM
    PUBLIC "${GTEST_SOURCE_DIR}/include"
    PRIVATE "${GTEST_SOURCE_DIR}")
target_link_libraries(shipwreck_gtest PUBLIC Threads::Threads)

add_library(shipwreck_gtest_main STATIC "${GTEST_SOURCE_DIR}/src/gtest_main.cc")
target_link_libraries(shipwreck_gtest_main PUBLIC shipwreck_gtest)
