#!/bin/bash

if [ "$1" == run ]; then
  gcc -o loesung -O3 -std=c11 -Wall -Werror -Wextra -Wpedantic -DNDEBUG loesung.c
  ./loesung
else
  echo run the program: run
  echo check for memory leaks: valgrind \(coming\)
fi