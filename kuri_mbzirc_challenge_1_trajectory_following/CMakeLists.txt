cmake_minimum_required(VERSION 2.8.3)
project(kuri_mbzirc_challenge_1_trajectory_following)


find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
)


################################################
## Declare ROS messages, services and actions ##
################################################




################################################
## Declare ROS dynamic reconfigure parameters ##
################################################



###################################
## catkin specific configuration ##
###################################


catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES kuri_mbzirc_challenge_1_trajectory_following
#  CATKIN_DEPENDS roscpp rospy
#  DEPENDS system_lib
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

add_executable(attitude_control src/landingOnstaticMarker.cpp)
target_link_libraries(attitude_control ${catkin_LIBRARIES})


#############
## Testing ##
#############


