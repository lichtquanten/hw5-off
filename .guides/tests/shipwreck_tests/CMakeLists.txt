set(SHIPWRECK_USER_SOURCE
    ${HW_DIR}/src/search.cpp
    ${HW_DIR}/src/seafloor_survey.cpp)

include_directories(${HW_DIR}/src ${HW_DIR}/.guides/support)

add_library(shipwreck_simulator STATIC
    ${HW_DIR}/.guides/support/scenario.cpp
    ${HW_DIR}/.guides/support/simulated_robot.cpp
    ${HW_DIR}/.guides/support/simulated_mothership.cpp)

add_source_problem(NAME shipwreck
    USER_SOURCE ${SHIPWRECK_USER_SOURCE}
    TEST_SOURCE basic_tests.cpp
    RUNTIME_TEST_SOURCE performance_tests.cpp)
target_link_libraries(shipwreck_tests shipwreck_simulator)
target_link_libraries(shipwreck_runtime_tests shipwreck_simulator)
