include_directories(. ${HW_DIR})

add_source_problem(NAME wordle
	USER_SOURCE ${HW_DIR}/wordle.cpp ${HW_DIR}/dict-eng.cpp
	TEST_SOURCE wordle-tests.cpp)

set_tests_properties( Wordle.Long PROPERTIES TIMEOUT 100)
set_tests_properties( Wordle.Instructor3 PROPERTIES TIMEOUT 100)
set_tests_properties( Wordle.Instructor2 PROPERTIES TIMEOUT 100)
set_tests_properties( Wordle.Instructor1 PROPERTIES TIMEOUT 100)

