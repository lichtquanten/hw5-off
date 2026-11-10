CXX := g++
CXXFLAGS := -std=c++17 -g -O2 -Wall -Wextra -I src

SOURCES := .guides/support/main.cpp .guides/support/scenario.cpp \
           .guides/support/simulated_robot.cpp .guides/support/simulated_mothership.cpp

# BEGIN - STUDENT SOURCE FILES
# Add new .cpp files here. Headers need no entry.

SOURCES += src/search.cpp src/seafloor_survey.cpp

# EXAMPLE - ADD YOUR OWN SOURCE FILE
# SOURCES += src/robot_selector.cpp

# END - STUDENT SOURCE FILES

all:
	mkdir -p build
	$(CXX) $(CXXFLAGS) $(SOURCES) -o build/shipwreck

run: all
	./build/shipwreck

benchmark: all
	./build/shipwreck --benchmark

init-tests: .guides/tests/Makefile

.guides/tests/Makefile: .guides/tests/CMakeLists.txt \
                      .guides/tests/shipwreck_tests/CMakeLists.txt \
                      .guides/tests/test_cmake_lib/CustomTests.cmake \
                      .guides/tests/test_cmake_lib/GoogleTest.cmake
	cd .guides/tests && cmake . -G "Unix Makefiles"

export TEST

check: .guides/tests/Makefile
	@python3 .guides/tests/check.py

references:
	@python3 .guides/tests/references/build_portable.py

clean:
	rm -rf build

.PHONY: all run benchmark init-tests check references clean
