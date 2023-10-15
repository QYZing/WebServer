cmake_minimum_required(VERSION 3.13)
project(dyingEpoller)


SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

set(LIBRARY_OUTPUT_PATH ${LIBPATH})
#generate static library
add_library(${LIBEPOLLER} STATIC Epoller.cpp)

