#Make 最低版本号要求
cmake_minimum_required(VERSION 2.8)
#项目信息
project(pollServer)

#include路径
include_directories(
${PROJECT_SOURCE_DIR}
${PROJECT_SOURCE_DIR}/muduo/base
${PROJECT_SOURCE_DIR}/muduo/net
${PROJECT_SOURCE_DIR}/Base
${PROJECT_SOURCE_DIR}/Http
${PROJECT_SOURCE_DIR}/Business
)

add_subdirectory(Http)
add_subdirectory(Base)
add_subdirectory(Monitor)

# 查找当前目录下的所有源文件
# 并将名称保存到 LIB_SRC 变量
aux_source_directory(. LIB_SRC)

#添加可执行文件
add_executable(pollServer ${LIB_SRC})

# 设置可执行文件生成路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}) 


find_library(LIB_MUDUOBASE muduo_base ${PROJECT_SOURCE_DIR}/libs)
find_library(LIB_MUDUONET muduo_net ${PROJECT_SOURCE_DIR}/libs)
find_library(LIB_SERVER server ${PROJECT_SOURCE_DIR}/libs)
#find_library(LIB_BUSINESS business ${PROJECT_SOURCE_DIR}/libs)
find_library(LIB_TEMP base  ${PROJECT_SOURCE_DIR}/libs)
find_library(LIB_BUSINESS business ${PROJECT_SOURCE_DIR}/libs) 
find_library(LIB_JPG jpeg ${PROJECT_SOURCE_DIR}/libs) 

#生成debug版本
SET(CMAKE_BUILD_TYPE "DEBUG") 
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -std=c++11  -fPIC -Wno-deprecated  -Wunused-value  -Wno-write-strings -g -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

link_directories(
${PROJECT_SOURCE_DIR}/libs
)

#其他系统库
TARGET_LINK_LIBRARIES(pollServer ${LIB_SERVER}
                        ${LIB_BUSINESS}
                        ${LIB_JPG}
                        ${LIB_TEMP}
                        ${LIB_MUDUOBASE}
                        ${LIB_MUDUONET}
                        avcodec
                        avformat
                        avutil
                        avdevice
                        avfilter
                        postproc
                        swresample
                        swscale
                        ${LIB_SBASE}
                        ${LIB_NET}
                        ${LIB_BASE}
                        glog
                        curl
                        pthread
                      )


