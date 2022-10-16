#!/bin/sh
gcc main.c -o main
chmod +x main
./main main2_in.txt main2_test.txt 10000 0.001