cmake_minimum_required(VERSION 3.13)
project(dyingThreadPool)

include_directories(${HEADPATH})
add_library(${LIBTHREADPOOL} STATIC ThreadPool.cpp)
