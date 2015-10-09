cmake_minimum_required(VERSION 2.8.3)
project(ras_image_manip)
find_package(
  catkin REQUIRED COMPONENTS
  cv_bridge
  image_transport
  roscpp
  sensor_msgs
  std_msgs
  ras_object_lib
)

 
find_package(OpenCV REQUIRED)
 
catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES ${PROJECT_NAME}
)
 
include_directories(
  include
  ${catkin_INCLUDE_DIRS}
  ${roscpp_INCLUDE_DIRS}
  ${OpenCV_INCLUDE_DIRS}
)
 
add_executable(image_manip  src/image_manip.cpp)

add_dependencies(image_manip
  ${${PROJECT_NAME}_EXPORTED_TARGETS} 
  ${catkin_EXPORTED_TARGETS}
)

 
target_link_libraries(image_manip
  ${roscpp_LIBRARIES}
  ${catkin_LIBRARIES}
  ${OpenCV_LIBS}
  ras_object_lib
)
