#Make 最低版本号要求
cmake_minimum_required(VERSION 2.8)
#项目信息
project(record)

#include路径
include_directories(
${PROJECT_SOURCE_DIR}
${PROJECT_SOURCE_DIR}/../
)


# 查找当前目录下的所有源文件
# 并将名称保存到 LIB_SRC 变量
aux_source_directory(. LIB_SRC)

#添加可执行文件
#add_executable(test ${LIB_SRC})
add_library(record STATIC ${LIB_SRC})


#设置可执行文件生成路径
#set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}) 
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/../libs)


#find_library(LIB_PATH H264Save ${PROJECT_SOURCE_DIR}/../)
find_library(LIBJPEG_PATH  jpeg ${PROJECT_SOURCE_DIR}/jpeg-9c/debug/lib)
 
#生成debug版本
SET(CMAKE_BUILD_TYPE "DEBUG") 
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -std=c++11  -fPIC  -Wno-write-strings -g -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

link_directories(/usr/local/lib)


#其他系统库
TARGET_LINK_LIBRARIES(record ${LIBJPEG_PATH}
                     avcodec
                     avformat
                     avutil
                     avdevice
                     avfilter
                     postproc
                     swresample
                     swscale)

