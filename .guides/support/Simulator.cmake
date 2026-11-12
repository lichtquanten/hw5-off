add_library(shipwreck_simulator STATIC
    "${HW_DIR}/.guides/support/scenario.cpp"
    "${HW_DIR}/.guides/support/simulated_robot.cpp"
    "${HW_DIR}/.guides/support/simulated_mothership.cpp")
target_include_directories(shipwreck_simulator PUBLIC "${HW_DIR}/src" "${HW_DIR}/.guides/support")
target_include_directories(shipwreck_simulator SYSTEM PUBLIC "${HW_DIR}/.guides/third_party")
