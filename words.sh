#!/bin/sh
gcc -xc++ -lstdc++ -shared-libgcc words.c -o output
./output wordsforproblem.txt
