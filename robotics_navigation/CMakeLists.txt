cmake_minimum_required(VERSION 2.8.3)
project(robotics_navigation)

find_package(catkin REQUIRED
)

catkin_package()

install(DIRECTORY config launch maps rviz
    DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
)
