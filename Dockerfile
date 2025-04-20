# debian 10 (GLIBC 2.28)
FROM debian:buster

# 安装必要的工具和库
RUN apt-get update && \
    apt-get install -y \
    build-essential \
    wget \
    gnupg \
    libx11-dev libxrandr-dev libxinerama-dev \
    libxcursor-dev libxi-dev \
    libglu1-mesa-dev freeglut3-dev mesa-common-dev

# 安装较新版本的CMake（3.20.0）
RUN wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | gpg --dearmor - | tee /usr/share/keyrings/kitware-archive-keyring.gpg >/dev/null && \
    echo 'deb [signed-by=/usr/share/keyrings/kitware-archive-keyring.gpg] https://apt.kitware.com/ubuntu/ bionic main' | tee /etc/apt/sources.list.d/kitware.list >/dev/null && \
    apt-get update && \
    apt-get install -y cmake && \
    rm -rf /var/lib/apt/lists/*

# 设置工作目录
WORKDIR /workspace

# 复制你的 C++ 工程到容器中
COPY . .

CMD ["/bin/bash", "-c", "./script/build_linux.sh"]

# 构建镜像
# docker build -t gpupixel .
# 执行镜像
# docker run -it --rm -v $(pwd)/output:/workspace/output -v $(pwd)/build:/workspace/build gpupixel
