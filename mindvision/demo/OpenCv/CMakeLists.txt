cmake_minimum_required( VERSION 2.8 )
project(mind)
 
# 添加c++ 11标准支持
set( CMAKE_CXX_FLAGS "-std=c++11" )
 
# 寻找OpenCV库
set(OpenCV_DIR /usr/local/include/opencv4)
find_package( OpenCV REQUIRED )
# 添加头文件
include_directories( ${OpenCV_INCLUDE_DIRS} 
						/home/coshe/mindvision/include/CameraApi.h
						/home/coshe/mindvision/include/CameraDefine.h
						/home/coshe/mindvision/include/CameraStatus.h)
 
add_executable( mind main.cpp )
# 链接OpenCV库
target_link_libraries( mind ${OpenCV_LIBS} )
