#!/bin/bash
# Library
gcc -c sum.c -o sum.o
gcc -c sum3.c -o sum3.o
ar rcs libsum.a sum.o sum3.o
# Binary
gcc -c main.c -o main.o
gcc main.o -L. -lsum -o main
