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

test:
	mkdir -p build
	$(CXX) $(CXXFLAGS) -I .support .support/search_tests.cpp \
		$(filter-out .support/main.cpp,$(SOURCES)) -o build/search_tests
	./build/search_tests

clean:
	rm -rf build

.PHONY: all run benchmark test clean
