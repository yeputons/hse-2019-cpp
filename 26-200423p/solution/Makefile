.PHONY: all test clean

CXX = g++
CXXFLAGS = -O2 -Wall -Wextra -Werror -std=c++17
LDFLAGS = 

all: test

test: test_any
	./test_any --reporters=verbose

test_any: test_any.cpp doctest_main.o any.h
	$(CXX) $(CXXFLAGS) $(LDFLAGS) test_any.cpp doctest_main.o -o $@

doctest_main.o: doctest_main.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf test_any doctest_main.o
