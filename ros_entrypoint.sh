#!/bin/bash
set -e

. /opt/ros/${ROS_DISTRO}/setup.bash

exec "$@"
