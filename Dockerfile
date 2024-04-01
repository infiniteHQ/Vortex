FROM archlinux:latest

RUN pacman -Syu --noconfirm && \
    pacman -S --noconfirm xorg-server nano xterm libx11 glfw make gcc spdlog vulkan-tools nvidia-utils glfw-x11 nvidia nvidia-utils nvidia-container-toolkit nvidia-container-runtime nvidia-smi


COPY build/bin/vortex /usr/local/bin/vortex
COPY build/assets/ /etc/vulkan/icd.d

WORKDIR /root

ENV DISPLAY=:0
ENV XDG_RUNTIME_DIR=/run/user/1000

CMD ["/bin/sh", "-c", "nvidia-smi && cd /root/vx/test && vortex -g"]
VOLUME build/vx:/root/vx
VOLUME /run/user/
VOLUME /tmp/.X11-unix