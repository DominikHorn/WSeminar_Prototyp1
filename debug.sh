#!/bin/bash

echo "Building..."
gcc -std=c99 -o jufo -g main.c testCases.c

echo "Executing..."
echo ""

gdb jufo

rm jufo
rm -r jufo.dSYM/ 
