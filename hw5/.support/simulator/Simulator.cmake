add_library(shipwreck_simulator STATIC
    "${HW_DIR}/.support/simulator/scenario.cpp"
    "${HW_DIR}/.support/simulator/simulated_robot.cpp"
    "${HW_DIR}/.support/simulator/simulated_mothership.cpp")
target_include_directories(shipwreck_simulator PUBLIC "${HW_DIR}" "${HW_DIR}/.support/simulator")
target_include_directories(shipwreck_simulator SYSTEM PUBLIC "${HW_DIR}/.support/third_party")
