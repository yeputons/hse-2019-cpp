.PHONY: all test clean

CXX = g++
CXXFLAGS = -O2 -Wall -Wextra -Werror -std=c++17
LDFLAGS = 

all: test

test: test_optional
	./test_optional --reporters=verbose

test_optional: test_optional.cpp doctest_main.o optional.h
	$(CXX) $(CXXFLAGS) $(LDFLAGS) test_optional.cpp doctest_main.o -o $@

doctest_main.o: doctest_main.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf test_optional doctest_main.o
