cmake_minimum_required(VERSION 3.14)
project(dyingLog)

#aux_source_directory(log.cpp SRC)
set(LIBRARY_OUTPUT_PATH ${LIBPATH})
#message(${SRC})
#generate static library
add_library(${LIBLOG} STATIC log.cpp)

add_executable(sample sample.cpp)
target_link_libraries(sample ${LIBLOG})
