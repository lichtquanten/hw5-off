include_directories(. ${HW_DIR})

add_source_problem(NAME schedwork
	USER_SOURCE ${HW_DIR}/schedwork.cpp
	TEST_SOURCE schedwork-tests.cpp)
