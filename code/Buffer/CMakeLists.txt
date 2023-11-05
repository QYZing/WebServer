cmake_minimum_required(VERSION 3.13)
project(dyingBuffer)

#generate static library
add_library(${LIBBUFFER} STATIC Buffer.cpp)
