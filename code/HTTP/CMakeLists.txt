cmake_minimum_required(VERSION 3.13)
project(dyingHTTP)


SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

aux_source_directory(./ SRC)
set(LIBRARY_OUTPUT_PATH ${LIBPATH})


include_directories(${HEADPATH})
message(${HEADPATH})

link_libraries(${LIBLOG})
link_libraries(${LIBBUFFER})

#generate static library
add_library(${LIBHTTP} STATIC ${SRC})
