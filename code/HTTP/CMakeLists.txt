cmake_minimum_required(VERSION 3.13)
project(dyingHTTP)

aux_source_directory(./ SRC)

include_directories(${HEADPATH})
message(${HEADPATH})

link_libraries(${LIBLOG})
link_libraries(${LIBBUFFER})

#generate static library
add_library(${LIBHTTP} STATIC ${SRC})
