cmake_minimum_required(VERSION 3.14)
project(WebServer)
#定义变量
#静态库生成路径
set(LIBPATH ${PROJECT_SOURCE_DIR}/build/lib)
#可执行程序的存储目录
set(EXECPATH ${PROJECT_SOURCE_DIR}/build/bin)
#头文件路径
set(HEADPATH ${PROJECT_SOURCE_DIR}/Log ${PROJECT_SOURCE_DIR}/ThreadPool)

set(LIBLOG log)
set(LIBTHREADPOOL threadpool)

add_subdirectory(${PROJECT_SOURCE_DIR}/Log)
add_subdirectory(${PROJECT_SOURCE_DIR}/ThreadPool)
add_subdirectory(${PROJECT_SOURCE_DIR}/test)