cmake_minimum_required(VERSION 2.8.3)
project(ras_object_detect)
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
 
add_executable(object_detector  src/object_detector.cpp)

add_dependencies(object_detector
  ${${PROJECT_NAME}_EXPORTED_TARGETS} 
  ${catkin_EXPORTED_TARGETS}
)

 
target_link_libraries(object_detector
  ${roscpp_LIBRARIES}
  ${catkin_LIBRARIES}
  ${OpenCV_LIBS}
  ras_object_lib
)
