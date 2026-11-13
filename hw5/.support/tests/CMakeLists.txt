cmake_minimum_required(VERSION 3.10)
project(ShipwreckTests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS FALSE)
add_compile_options(-g -O2 -Wall -Wextra)

get_filename_component(HW_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)
set(SOLUTION_DIR "${HW_DIR}" CACHE PATH "")

find_path(GTEST_SOURCE_DIR src/gtest-all.cc HINTS /usr/src/googletest/googletest /usr/src/gtest)
if(NOT GTEST_SOURCE_DIR)
    message(FATAL_ERROR "Google Test sources are missing. Install libgtest-dev or set -DGTEST_SOURCE_DIR=<path>.")
endif()
find_package(Threads REQUIRED)
add_library(gtest_main STATIC "${GTEST_SOURCE_DIR}/src/gtest-all.cc" "${GTEST_SOURCE_DIR}/src/gtest_main.cc")
target_include_directories(gtest_main SYSTEM PUBLIC "${GTEST_SOURCE_DIR}/include" PRIVATE "${GTEST_SOURCE_DIR}")
target_link_libraries(gtest_main PUBLIC Threads::Threads)

set(SIMULATOR_DIR "${HW_DIR}/.support/simulator")
add_library(simulator STATIC
    "${SIMULATOR_DIR}/scenario.cpp"
    "${SIMULATOR_DIR}/simulated_robot.cpp"
    "${SIMULATOR_DIR}/simulated_mothership.cpp")
target_include_directories(simulator PUBLIC "${SIMULATOR_DIR}" PRIVATE "${HW_DIR}")

file(GLOB_RECURSE SOLUTION_FILES RELATIVE "${SOLUTION_DIR}" "${SOLUTION_DIR}/*.cpp")
list(FILTER SOLUTION_FILES EXCLUDE REGEX "(^|/)(\\.support|\\.git|build)/")
set(SOLUTION_SOURCES "")
foreach(source IN LISTS SOLUTION_FILES)
    list(APPEND SOLUTION_SOURCES "${SOLUTION_DIR}/${source}")
endforeach()
add_library(solution STATIC ${SOLUTION_SOURCES})
target_include_directories(solution PUBLIC "${SOLUTION_DIR}" "${HW_DIR}")
target_include_directories(solution SYSTEM PUBLIC "${HW_DIR}/.support/third_party")
target_compile_definitions(solution PRIVATE main=disabled_main)

foreach(name basic_tests performance_tests)
    add_executable(${name} "shipwreck_tests/${name}.cpp")
    target_link_libraries(${name} PRIVATE simulator solution gtest_main)
endforeach()
