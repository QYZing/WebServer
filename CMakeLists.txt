cmake_minimum_required(VERSION 3.13)
project(WebServer)

#静态库生成路径
set(LIBPATH ${PROJECT_SOURCE_DIR}/build/lib)
#可执行程序的存储目录
set(EXECPATH ${PROJECT_SOURCE_DIR}/build/bin)
#头文件路径
set(HEADPATH ${PROJECT_SOURCE_DIR}/Log
        ${PROJECT_SOURCE_DIR}/ThreadPool
        ${PROJECT_SOURCE_DIR}/Timer
        ${PROJECT_SOURCE_DIR}/googletest/googletest/include
        ${PROJECT_SOURCE_DIR}/Buffer)

set(LIBLOG log)
set(LIBTHREADPOOL threadpool)
set(LIBTIMER timer)
set(LIBEPOLLER epoller)
set(LIBBUFFER buffer)

add_subdirectory(${PROJECT_SOURCE_DIR}/googletest)
add_subdirectory(${PROJECT_SOURCE_DIR}/Log)
add_subdirectory(${PROJECT_SOURCE_DIR}/Epoller)
add_subdirectory(${PROJECT_SOURCE_DIR}/ThreadPool)
add_subdirectory(${PROJECT_SOURCE_DIR}/Timer)
add_subdirectory(${PROJECT_SOURCE_DIR}/Buffer)
add_subdirectory(${PROJECT_SOURCE_DIR}/test)