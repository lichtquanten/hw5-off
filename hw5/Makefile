CXX := g++
CXXFLAGS := -std=c++17 -g -O2 -Wall -Wextra -I . -isystem .support/third_party

# Find student .cpp files recursively, excluding support files and generated output.
STUDENT_SOURCES := $(sort $(shell find . -type d \( -name .support -o -name .git -o -name build \) \
                   -prune -o -type f -name '*.cpp' -print))
SOURCES := .support/simulator/main.cpp .support/simulator/scenario.cpp \
           .support/simulator/simulated_robot.cpp .support/simulator/simulated_mothership.cpp \
           $(STUDENT_SOURCES)

all:
	mkdir -p build
	$(CXX) $(CXXFLAGS) $(SOURCES) -o build/shipwreck

smoke-test: all
	./build/shipwreck

test:
	@python3 -B .support/tests/check.py

clean:
	rm -rf build
	find . -type d -name __pycache__ -prune -exec rm -rf {} +

.PHONY: all smoke-test test clean
