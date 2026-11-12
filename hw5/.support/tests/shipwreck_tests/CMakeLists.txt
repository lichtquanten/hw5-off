file(GLOB_RECURSE STUDENT_FILES RELATIVE "${HW_DIR}" "${HW_DIR}/*.cpp")
list(FILTER STUDENT_FILES EXCLUDE REGEX "(^|/)(\\.support|\\.git|build)/")
set(STUDENT_SOURCES "")
foreach(source IN LISTS STUDENT_FILES)
    list(APPEND STUDENT_SOURCES "${HW_DIR}/${source}")
endforeach()
add_library(hw5_shipwreck STATIC ${STUDENT_SOURCES})
target_include_directories(hw5_shipwreck PUBLIC "${HW_DIR}")
target_include_directories(hw5_shipwreck SYSTEM PUBLIC "${HW_DIR}/.support/third_party")
target_compile_definitions(hw5_shipwreck PUBLIC main=disabled_main)

include("${HW_DIR}/.support/simulator/Simulator.cmake")

add_executable(shipwreck_tests basic_tests.cpp)
target_link_libraries(shipwreck_tests PRIVATE hw5_shipwreck shipwreck_simulator shipwreck_gtest_main)

add_executable(shipwreck_runtime_tests performance_tests.cpp)
target_link_libraries(shipwreck_runtime_tests PRIVATE hw5_shipwreck shipwreck_simulator shipwreck_gtest_main)
