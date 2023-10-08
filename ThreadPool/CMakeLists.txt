cmake_minimum_required(VERSION 3.14)
project(dyingThreadPool)

SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

#aux_source_directory(log.cpp SRC)
set(LIBRARY_OUTPUT_PATH ${LIBPATH})
#message(${SRC})
#generate static library

include_directories(${HEADPATH})

add_library(${LIBTHREADPOOL} STATIC ThreadPool.cpp)

add_executable(test sample.cpp)
target_link_libraries(test ${LIBLOG})
