CXX := g++
CXXFLAGS := -std=c++17 -g -O2 -Wall -Wextra -I src -isystem .guides/third_party

# Every .cpp file under src/ is compiled recursively. Headers need no entry.
SOURCES := .guides/support/main.cpp .guides/support/scenario.cpp \
           .guides/support/simulated_robot.cpp .guides/support/simulated_mothership.cpp \
           $(sort $(shell find src -type f -name '*.cpp'))

all:
	mkdir -p build
	$(CXX) $(CXXFLAGS) $(SOURCES) -o build/shipwreck

smoke-test: all
	./build/shipwreck

test:
	@python3 .guides/tests/check.py

clean:
	rm -rf build

.PHONY: all smoke-test test clean
