CXX := g++
CXXFLAGS := -std=c++17 -g -O2 -Wall -Wextra -I src

# Every .cpp file in src/ is compiled. Headers need no entry.
SOURCES := .guides/support/main.cpp .guides/support/scenario.cpp \
           .guides/support/simulated_robot.cpp .guides/support/simulated_mothership.cpp \
           $(wildcard src/*.cpp)

all:
	mkdir -p build
	$(CXX) $(CXXFLAGS) $(SOURCES) -o build/shipwreck

run: all
	./build/shipwreck

benchmark: all
	./build/shipwreck --benchmark

check:
	@python3 .guides/tests/check.py

clean:
	rm -rf build

.PHONY: all run benchmark check clean
