cmake_minimum_required(VERSION 2.8.3)
project(ras_object_detect)
set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}")


find_package(
  catkin REQUIRED COMPONENTS
  cv_bridge
  image_transport
  roscpp
  sensor_msgs
  std_msgs
  message_generation
  ras_object_lib
)

 
find_package(OpenCV REQUIRED)

generate_messages(
  DEPENDENCIES
  std_msgs
  sensor_msgs
)


catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES ${PROJECT_NAME}
)

#add_service_files(
#FILES
#ImageTransfer.srv
#)
 
 
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


add_executable(object_detector_test  src/object_detector_test.cpp)


add_dependencies(object_detector_test
  ${${PROJECT_NAME}_EXPORTED_TARGETS} 
  ${catkin_EXPORTED_TARGETS}
)


target_link_libraries(object_detector_test
  ${roscpp_LIBRARIES}
  ${catkin_LIBRARIES}
  ${OpenCV_LIBS}
  ras_object_lib
)


add_executable(shape_detector  src/shape_detector.cpp)


add_dependencies(shape_detector
  ${${PROJECT_NAME}_EXPORTED_TARGETS} 
  ${catkin_EXPORTED_TARGETS}
)


target_link_libraries(shape_detector
  ${roscpp_LIBRARIES}
  ${catkin_LIBRARIES}
  ${OpenCV_LIBS}
  ras_object_lib
)


add_executable(color_detect  src/color_detect.cpp)

add_dependencies(color_detect
  ${${PROJECT_NAME}_EXPORTED_TARGETS} 
  ${catkin_EXPORTED_TARGETS}
)
 
target_link_libraries(color_detect
  ${roscpp_LIBRARIES}
  ${catkin_LIBRARIES}
  ${OpenCV_LIBS}
  ras_object_lib
)
  
