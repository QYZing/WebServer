cmake_minimum_required(VERSION 3.13)
project(dyingTimer)

#aux_source_directory(log.cpp SRC)
set(LIBRARY_OUTPUT_PATH ${LIBPATH})
#message(${SRC})
#generate static library

include_directories(${HEADPATH})

link_libraries(${LIBLOG})

add_library(${LIBTIMER} STATIC Timer.cpp)

add_executable(TimerSample sample.cpp)
target_link_libraries(TimerSample ${LIBTIMER})
