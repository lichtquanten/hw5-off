CXX := g++
CXXFLAGS := -std=c++17 -g -O2 -Wall -Wextra -I src

SOURCES := .support/main.cpp .support/scenario.cpp \
           .support/simulated_robot.cpp .support/simulated_mothership.cpp \
           .support/search_driver.cpp

# BEGIN STUDENT SOURCE FILES
# Add new .cpp files here. Headers need no entry.

SOURCES += src/search.cpp src/seafloor_survey.cpp

# Uncomment when using the class example in src/search_stats.h and .cpp:
# SOURCES += src/search_stats.cpp

# END STUDENT SOURCE FILES

all:
	mkdir -p build
	$(CXX) $(CXXFLAGS) $(SOURCES) -o build/shipwreck

run: all
	./build/shipwreck

benchmark: all
	./build/shipwreck --benchmark

test:
	mkdir -p build
	$(CXX) $(CXXFLAGS) -I .support .support/search_driver_tests.cpp \
		.support/search_driver.cpp .support/scenario.cpp \
		.support/simulated_robot.cpp .support/simulated_mothership.cpp \
		-o build/search_driver_tests
	./build/search_driver_tests

clean:
	rm -rf build

.PHONY: all run benchmark test clean
