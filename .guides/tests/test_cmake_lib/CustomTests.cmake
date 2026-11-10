# Register the Google Test cases used by this assignment with CTest.
function(add_executable_gtests executable use_valgrind)
    get_target_property(sources ${executable} SOURCES)
    foreach(source ${sources})
        get_filename_component(source_path "${source}" ABSOLUTE BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
        set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${source_path}")
        file(STRINGS "${source_path}" declarations REGEX "^[ \t]*TEST_F\\(")
        foreach(declaration ${declarations})
            string(REGEX REPLACE "^[ \t]*TEST_F\\([ \t]*([A-Za-z_0-9]+)[ \t]*,[ \t]*([A-Za-z_0-9]+)[ \t]*\\).*" "\\1.\\2" test_name "${declaration}")
            if(use_valgrind)
                add_test(NAME ${test_name} COMMAND "${VALGRIND}" ${VALGRIND_ARGS}
                    $<TARGET_FILE:${executable}> --gtest_filter=${test_name})
            else()
                add_test(NAME ${test_name} COMMAND ${executable} --gtest_filter=${test_name})
            endif()
            add_custom_target(debug-${test_name}
                COMMAND gdb --args $<TARGET_FILE:${executable}> --gtest_filter=${test_name}
                WORKING_DIRECTORY "${HW_DIR}" VERBATIM)
        endforeach()
    endforeach()
endfunction()

find_package(PythonInterp 3.6.9 REQUIRED)
add_custom_target(check
    COMMAND "${PYTHON_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/check.py"
        --build-dir "${CMAKE_BINARY_DIR}"
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}" VERBATIM)

file(GENERATE OUTPUT "${CMAKE_BINARY_DIR}/CTestCustom.cmake" CONTENT
    "set(CTEST_CUSTOM_MAXIMUM_PASSED_TEST_OUTPUT_SIZE 307200)\n")
