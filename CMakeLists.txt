cmake_minimum_required(VERSION 3.22)
project(Neuron_Liniowy C)

set(CMAKE_C_STANDARD 11)

add_executable(Neuron_Liniowy
        main.c)

add_executable(madaline
        main2.c)

file(COPY main2_in.txt DESTINATION ${CMAKE_BINARY_DIR})
file(COPY main2_test.txt DESTINATION ${CMAKE_BINARY_DIR})
