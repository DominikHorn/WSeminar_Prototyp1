#!/bin/bash

if [ ! -d "~/Desktop/build/" ]; then
   echo "missing build directory at Desktop, creating it automatically..."
   mkdir ~/Desktop/build/
fi

echo "Building..."
gcc -std=c99 -o ~/Desktop/build/jufo main.c testCases.c
