
cmake_minimum_required(VERSION 2.8.3)
project(ras_object_lib)

set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}")
find_package(
  catkin REQUIRED COMPONENTS
  cv_bridge
  image_transport
  roscpp
  sensor_msgs
  std_msgs
  message_generation
)
 
find_package(OpenCV REQUIRED)
add_service_files(
  FILES
  Image_Transfer.srv
  Depth_Transfer.srv 
)


generate_messages(
DEPENDENCIES
std_msgs
sensor_msgs
)


 
catkin_package(
  INCLUDE_DIRS include
 CATKIN_DEPENDS message_runtime std_msgs sensor_msgs
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
  src/image_color_range.cpp
  src/image_utils.cpp
#  src/image_manip.cpp
)


target_link_libraries(ras_object_lib
  ${roscpp_LIBRARIES}
  ${catkin_LIBRARIES}
  ${OpenCV_LIBS}
)
