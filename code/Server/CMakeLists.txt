cmake_minimum_required(VERSION 3.13)
project(dyingWebServer)

link_libraries(${LIBLOG})
link_libraries(${LIBEPOLLER})
link_libraries(${LIBTIMER})
link_libraries(${LIBHTTP})
link_libraries(${LIBTHREADPOOL})

include_directories(${HEADPATH})

#generate static library
add_library(${LIBWEBSERVER} STATIC WebServer.cpp)
