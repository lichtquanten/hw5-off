add_library(hw5_shipwreck STATIC
    "${HW_DIR}/src/search.cpp"
    "${HW_DIR}/src/seafloor_survey.cpp")
target_include_directories(hw5_shipwreck PUBLIC "${HW_DIR}/src")
target_compile_definitions(hw5_shipwreck PUBLIC main=disabled_main)

add_library(shipwreck_simulator STATIC
    "${HW_DIR}/.guides/support/scenario.cpp"
    "${HW_DIR}/.guides/support/simulated_robot.cpp"
    "${HW_DIR}/.guides/support/simulated_mothership.cpp")
target_include_directories(shipwreck_simulator PUBLIC "${HW_DIR}/src" "${HW_DIR}/.guides/support")

add_executable(shipwreck_tests basic_tests.cpp)
target_link_libraries(shipwreck_tests PRIVATE hw5_shipwreck shipwreck_simulator shipwreck_gtest_main)
add_executable_gtests(shipwreck_tests TRUE)

add_executable(shipwreck_runtime_tests performance_tests.cpp)
target_link_libraries(shipwreck_runtime_tests PRIVATE hw5_shipwreck shipwreck_simulator shipwreck_gtest_main)
add_executable_gtests(shipwreck_runtime_tests FALSE)
