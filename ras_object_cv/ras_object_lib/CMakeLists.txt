cmake_minimum_required(VERSION 2.8.3)
project(ras_object_lib)
find_package(
  catkin REQUIRED COMPONENTS
  cv_bridge
  image_transport
  roscpp
  sensor_msgs
  std_msgs
)
 
find_package(OpenCV REQUIRED)
 
catkin_package(
  INCLUDE_DIRS include
  LIBRARIES ${PROJECT_NAME}
)
 
include_directories(
  include
  ${roscpp_INCLUDE_DIRS}
  ${OpenCV_INCLUDE_DIRS}
)
 
add_library(ras_object_lib
  src/image_converter.cpp
  src/image_normalise.cpp
  src/image_morph.cpp
#  src/image_manip.cpp
)
 
target_link_libraries(ras_object_lib
  ${roscpp_LIBRARIES}
  ${catkin_LIBRARIES}
  ${OpenCV_LIBS}
)
