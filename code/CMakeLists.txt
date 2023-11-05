cmake_minimum_required(VERSION 3.13)
project(WebServer)

SET(LIBRARY_OUTPUT_PATH  ${LIBPATH})

add_subdirectory(Config)
add_subdirectory(Log)
add_subdirectory(Buffer)
add_subdirectory(Epoller)
add_subdirectory(Timer)
add_subdirectory(HTTP)
add_subdirectory(ThreadPool)
add_subdirectory(Server)
