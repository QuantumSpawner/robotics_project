FROM osrf/ros:noetic-desktop-full-focal

# install general packages
RUN apt-get update && apt-get upgrade -y \
    && apt-get install --no-install-recommends -y \
    bash-completion \
    ca-certificates \
    curl \
    git \
    htop \
    lsb-release \
    net-tools \
    python3-pip \
    tmux \
    unzip \
    vim \
    wget

# install dependencies using rosdep
COPY ./ /tmp/src
RUN . /opt/ros/${ROS_DISTRO}/setup.sh \
    && rosdep install --from-paths /tmp/src --ignore-src -r -y

# adding user "docker" and add it to sudo group
# password "docker" for root and user "docker"
RUN useradd --create-home --shell /bin/bash docker \
    && usermod -aG sudo docker \
    && echo "docker\ndocker" | passwd \
    && echo "docker\ndocker" | passwd docker

# change to user "docker" and change to home directory
USER docker
ENV USER docker
WORKDIR /home/docker

# ros setup
# update rosdep for user "docker"
# prebuild ws once
RUN . /opt/ros/${ROS_DISTRO}/setup.sh \
    && rosdep update --rosdistro ${ROS_DISTRO} \
    && mkdir -p ws/src && cd ws && catkin_make

# copy and modify .bashrc for user "docker"
RUN cp /etc/skel/.bashrc ~ \
    && echo "export PATH=${PATH}/~/.local/bin" >> ~/.bashrc \
    && echo "source /opt/ros/${ROS_DISTRO}/setup.bash" >> ~/.bashrc \
    && echo "source ~/ws/devel/setup.bash" >> ~/.bashrc

# setup entrypoint
COPY ./ros_entrypoint.sh /
ENTRYPOINT ["/ros_entrypoint.sh"]
CMD ["bash"]
