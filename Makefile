CXX := g++
CXXFLAGS := -std=c++17 -g -O2 -Wall -Wextra -I src

SOURCES := .support/main.cpp .support/scenario.cpp \
           .support/simulated_robot.cpp .support/simulated_mothership.cpp

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

init-tests: hw5_tests/Makefile

hw5_tests/Makefile:
	cmake -S hw5_tests -B hw5_tests -G "Unix Makefiles"

export TEST

check: hw5_tests/Makefile
	@python3 hw5_tests/check.py

references:
	@python3 hw5_tests/references/build.py

clean:
	rm -rf build

.PHONY: all run benchmark init-tests check references clean
