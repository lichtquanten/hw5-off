cmake_minimum_required(VERSION 3.10)
project(ShipwreckReferences LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS FALSE)
set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_DEBUG "-g3 -O0")
add_compile_options(-Wall)
find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

if(NOT REFERENCE MATCHES "^(starter|instructor)$" OR NOT REFERENCE_FINGERPRINT)
    message(FATAL_ERROR "Use the root 'make references' command to configure this project.")
endif()
get_filename_component(HW_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../.." ABSOLUTE)
set(REFERENCE_SRC "${CMAKE_CURRENT_SOURCE_DIR}/${REFERENCE}/src")
if(NOT EXISTS "${REFERENCE_SRC}/search.h" OR NOT EXISTS "${REFERENCE_SRC}/search.cpp")
    message(FATAL_ERROR "Missing ${REFERENCE} src/search.h or src/search.cpp")
endif()
file(GLOB_RECURSE REFERENCE_SOURCES "${REFERENCE_SRC}/*.cpp")
configure_file(reference_info.h.in reference_info.h @ONLY)

add_library(reference_solution STATIC ${REFERENCE_SOURCES})
target_include_directories(reference_solution PUBLIC "${REFERENCE_SRC}" "${HW_DIR}/src")
add_library(reference_simulator STATIC
    "${HW_DIR}/.guides/support/scenario.cpp"
    "${HW_DIR}/.guides/support/simulated_robot.cpp"
    "${HW_DIR}/.guides/support/simulated_mothership.cpp")
target_include_directories(reference_simulator PUBLIC "${HW_DIR}/src" "${HW_DIR}/.guides/support")

function(reference_binary target source)
    add_executable(${target} "${source}" reference_main.cpp)
    target_include_directories(${target} PRIVATE "${REFERENCE_SRC}" "${CMAKE_CURRENT_BINARY_DIR}")
    target_link_libraries(${target} PRIVATE reference_solution reference_simulator GTest::GTest Threads::Threads)
endfunction()
reference_binary(shipwreck_reference "${HW_DIR}/.guides/tests/shipwreck_tests/performance_tests.cpp")
reference_binary(reference_checks "${HW_DIR}/.guides/tests/shipwreck_tests/basic_tests.cpp")
target_sources(reference_checks PRIVATE tests/performance_scenario_tests.cpp)
target_include_directories(reference_checks PRIVATE "${HW_DIR}/.guides/tests/shipwreck_tests")

if(REFERENCE STREQUAL "instructor")
    target_sources(reference_checks PRIVATE tests/instructor_history_tests.cpp)
endif()
