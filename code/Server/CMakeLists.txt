cmake_minimum_required(VERSION 3.13)
project(dyingWebServer)

set(LIBRARY_OUTPUT_PATH ${LIBPATH})

SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

link_libraries(${LIBLOG})
link_libraries(${LIBEPOLLER})
link_libraries(${LIBTIMER})
link_libraries(${LIBHTTP})
link_libraries(${LIBTHREADPOOL})

include_directories(${HEADPATH})

#generate static library
add_library(${LIBWEBSERVER} STATIC WebServer.cpp)
