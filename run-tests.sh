#!/usr/bin/env sh
clang++ -std=c++17 -o test test.cpp test-core.o
./test --durations yes
