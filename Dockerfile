FROM archlinux:latest

RUN pacman -Syu --noconfirm
RUN pacman -S --noconfirm xorg-server nano xterm libx11 git glfw make gcc spdlog vulkan-tools nvidia-utils wget glfw-x11 vulkan-devel cmake libxinerama unzip nvidia nvidia-utils nlohmann-json nvidia-container-toolkit
RUN mkdir ~/Downloads && cd ~/Downloads && wget https://infinite.si/_b/VortexMaker.zip
RUN cd ~/Downloads && unzip VortexMaker.zip && cd ~/Downloads/VortexMaker && mkdir build && cd build && cmake .. && make install
    

WORKDIR /root

ENV DISPLAY=:0
ENV XDG_RUNTIME_DIR=/run/user/1000


CMD ["/bin/sh", "-c", "nvidia-smi && cd /root/vx/test && vortex -g"]
VOLUME build/vx:/root/vx
VOLUME /run/user/
VOLUME /tmp/.X11-unix