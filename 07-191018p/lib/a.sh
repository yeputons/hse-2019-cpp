#!/bin/bash
gcc -c sum.c -o sum.o
ar rcs libsum.a sum.o
gcc -c main.c -o main.o -lsum

