cmake_minimum_required(VERSION 3.13)
project(dyingLog)

#aux_source_directory(log.cpp SRC)
#message(${SRC})
#generate static library
add_library(${LIBLOG} STATIC Log.cpp)

add_executable(LogSample sample.cpp)
target_link_libraries(LogSample ${LIBLOG})
