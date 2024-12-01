cmake_minimum_required(VERSION 2.8.3)
project(robotics_bringup)

find_package(catkin REQUIRED COMPONENTS
    nav_msgs
    roscpp
)
find_package(Boost REQUIRED)

catkin_package(
    CATKIN_DEPENDS nav_msgs roscpp
)

include_directories(
    ${catkin_INCLUDE_DIRS}
)

add_executable(odom_covariance_node src/odom_covariance_node.cpp)
target_link_libraries(odom_covariance_node ${catkin_LIBRARIES})

install(DIRECTORY launch rviz
    DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
)
