cmake_minimum_required(VERSION 3.13)
project(dyingEpoller)

#generate static library
add_library(${LIBEPOLLER} STATIC Epoller.cpp)

