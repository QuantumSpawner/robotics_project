cmake_minimum_required(VERSION 2.8.3)
project(robotics_bringup)

find_package(catkin
)

catkin_package(
)

install(DIRECTORY launch rviz
    DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION})
