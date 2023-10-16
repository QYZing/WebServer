cmake_minimum_required(VERSION 3.13)
project(test1)
#
#add_definitions(-DDEBUG)
#搜索源文件
aux_source_directory(./ SRC)
include_directories(${HEADPATH})

# include static lib path
link_directories(${LIBPATH})

# linking static library , remember contain lib path

set(EXECUTABLE_OUTPUT_PATH  ${TESTPATH})

macro (add_test_exec exec_name)
    add_executable ("TEST_${exec_name}" "TEST_${exec_name}.cpp")
    target_link_libraries("TEST_${exec_name}" ${ARGN} gtest)
endmacro (add_test_exec)

add_test_exec (Buffer ${LIBBUFFER})
add_test_exec (Timer ${LIBTIMER})
add_test_exec (ThreadPool ${LIBTHREADPOOL})
add_test_exec (WebServer)