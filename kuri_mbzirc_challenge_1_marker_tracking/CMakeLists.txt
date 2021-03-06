cmake_minimum_required(VERSION 2.8.3)
project(kuri_mbzirc_challenge_1_marker_tracking)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  std_msgs
  sensor_msgs
  visp_bridge
  cv_bridge
  message_generation
)

find_package(OpenCV 2.4.8 REQUIRED)

################################################
## Declare ROS messages, services and actions ##
################################################

add_message_files(
  DIRECTORY msg
  FILES TrackerData.msg
)

generate_messages(
  DEPENDENCIES
  std_msgs
)


################################################
## Declare ROS dynamic reconfigure parameters ##
################################################


###################################
## catkin specific configuration ##
###################################

catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES kuri_mbzirc_challenge_1_marker_tracking
#  CATKIN_DEPENDS roscpp rospy
#  DEPENDS system_lib
  CATKIN_DEPENDS message_runtime
)

###########
## Build ##
###########

include_directories(
  ${catkin_INCLUDE_DIRS}
)

#############
## Install ##
#############

add_executable(visp-tracker src/visptracker.cpp src/mark_tracker.cpp src/detector/landing_mark_detection.cpp src/simplemotionfilter.cpp)
add_dependencies(visp-tracker kuri_mbzirc_challenge_1_marker_tracking_generate_messages_cpp)
target_link_libraries(visp-tracker ${catkin_LIBRARIES})
target_link_libraries(visp-tracker ${OpenCV_LIBRARIES})
  

#############
## Testing ##
#############


