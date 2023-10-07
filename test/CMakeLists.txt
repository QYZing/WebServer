cmake_minimum_required(VERSION 3.14)
project(test1)
#
#add_definitions(-DDEBUG)
#搜索源文件
aux_source_directory(./ SRC)
#include_directories(${HEADPATH})
# include static lib path
link_directories(${LIBPATH})
# linking static library , remember contain lib path
link_libraries(${LIBLOG})
set(EXECUTABLE_OUTPUT_PATH  ${EXECPATH})
message("set out path" ${EXECPATH})
if(SRC)
    add_executable(test ${SRC})
    message("生成可执行文件 test")
else()
    message("没有指定 SRC 变量，不生成可执行文件")
endif()
