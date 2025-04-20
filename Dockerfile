# debian 10 (GLIBC 2.28)
FROM debian:buster

# 安装必要的工具和库
RUN apt-get update && \
    apt-get install -y \
    build-essential \
    cmake \
    libx11-dev libxrandr-dev libxinerama-dev \
    libxcursor-dev libxi-dev \
    libglu1-mesa-dev freeglut3-dev mesa-common-dev \
    && rm -rf /var/lib/apt/lists/*

# 设置工作目录
WORKDIR /workspace

# 复制你的 C++ 工程到容器中
COPY . .

# 设置默认命令为进入 build 目录并运行 cmake 和 make
# CMD ["/bin/bash", "-c", "cmake -B build -S src -DCMAKE_BUILD_TYPE=Release && cd build &&  make -j4"]

# 构建镜像
# docker build -t gpupixel .
# 执行镜像
# docker run -it --rm -v $(pwd)/output:/workspace/output -v $(pwd)/build:/workspace/build gpupixel
