#Make 最低版本号要求
cmake_minimum_required(VERSION 2.8)
#项目信息
project(server)

#include路径
include_directories(
${PROJECT_SOURCE_DIR}
${PROJECT_SOURCE_DIR}/../
${PROJECT_SOURCE_DIR}/../Base
${PROJECT_SOURCE_DIR}/../muduo
${PROJECT_SOURCE_DIR}/../muduo/net
${PROJECT_SOURCE_DIR}/../muduo/base
)


# 查找当前目录下的所有源文件
# 并将名称保存到 LIB_SRC 变量
aux_source_directory(. LIB_SRC)

find_library(LIB_PATH base ${PROJECT_SOURCE_DIR}/../libs)

#添加可执行文件
ADD_LIBRARY(server ${LIB_SRC})

# 设置可执行文件生成路径
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/../libs) 

#生成debug版本
SET(CMAKE_BUILD_TYPE "DEBUG") 
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -std=c++11  -fPIC -Wno-deprecated  -Wno-write-strings -g -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")


link_directories(${PROJECT_SOURCE_DIR}/../libs)

#其他系统库
TARGET_LINK_LIBRARIES(server
                      ${LIB_PATH}
                      glog 
                      curl 
                      pthread
                      )

