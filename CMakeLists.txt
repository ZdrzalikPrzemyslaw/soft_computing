cmake_minimum_required(VERSION 3.22)
project(Neuron_Liniowy C)

set(CMAKE_C_STANDARD 11)

add_executable(Neuron_Liniowy
        ex_1.c)

add_executable(madaline
        ex_2.c)

file(COPY ex_2_train.txt DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ex_2_test.txt DESTINATION ${CMAKE_BINARY_DIR})
