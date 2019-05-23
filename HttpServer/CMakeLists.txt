cmake_minimum_required(VERSION 3.9)
add_definitions(-DOS_MACOSX)
project(echo)

include_directories("${PROJECT_SOURCE_DIR}")
set(CMAKE_CXX_STANDARD 11)

aux_source_directory(handy HANDY_SRCS)
add_library(handy
        ${HANDY_SRCS})
		
add_executable(http-hello
        http-hello.cc)
target_link_libraries(http-hello handy)

add_executable(timer
        timer.cc)
target_link_libraries(timer handy)
