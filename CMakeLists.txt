cmake_minimum_required(VERSION 3.13)
project(WebServer)


SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -pg -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

#静态库生成路径
set(LIBPATH ${PROJECT_SOURCE_DIR}/build/lib)
#可执行程序的存储目录
set(EXECPATH ${PROJECT_SOURCE_DIR}/build/bin)
#头文件路径
set(HEADPATH ${PROJECT_SOURCE_DIR}/code/Log
        ${PROJECT_SOURCE_DIR}/code/ThreadPool
        ${PROJECT_SOURCE_DIR}/code/Timer
        ${PROJECT_SOURCE_DIR}/code/Epoller
        ${PROJECT_SOURCE_DIR}/googletest/googletest/include
        ${PROJECT_SOURCE_DIR}/code/Buffer
        ${PROJECT_SOURCE_DIR}/code/HTTP
        ${PROJECT_SOURCE_DIR}/code/Server
        ${PROJECT_SOURCE_DIR}/code/Config
        ${PROJECT_SOURCE_DIR}/yaml-cpp/include
        )
set(TESTPATH ${PROJECT_SOURCE_DIR}/build/test)

set(LIBLOG log)
set(LIBTHREADPOOL threadpool)
set(LIBTIMER timer)
set(LIBEPOLLER epoller)
set(LIBBUFFER buffer)
set(LIBHTTP http)
set(LIBWEBSERVER webserver)

add_subdirectory(${PROJECT_SOURCE_DIR}/code)
add_subdirectory(${PROJECT_SOURCE_DIR}/googletest)
add_subdirectory(${PROJECT_SOURCE_DIR}/test)

link_directories(${LIBPATH})
include_directories(${HEADPATH})

add_executable(main main.cpp
)
target_link_libraries(main ${LIBWEBSERVER})
target_link_libraries(main ${PROJECT_SOURCE_DIR}/yaml-cpp/lib/libyaml-cpp.a)

