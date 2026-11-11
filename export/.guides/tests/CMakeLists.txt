cmake_minimum_required(VERSION 3.10)
project(CS104-HW5-Test-Suite LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS FALSE)
set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_DEBUG "-g3 -O0")
add_compile_options(-Wall)

get_filename_component(HW_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)

include(GoogleTest.cmake)
find_program(VALGRIND NAMES valgrind)
if(NOT VALGRIND)
    message(FATAL_ERROR "Valgrind is required to check memory errors and peak memory usage.")
endif()

add_subdirectory(shipwreck_tests)
