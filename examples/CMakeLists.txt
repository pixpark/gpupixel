# 设置 cmkae 最低版本
CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)

# 设置引入的 cmake 文件路径为当前目录
SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")

# 定义当前模块的名称
SET(APP_PROJECT_NAME "gpupixel_app")
PROJECT(${APP_PROJECT_NAME})

# 定义是否编译 DEBUG 版本(也可以在执行 cmake 指令的时候设置：cmake -DBUILD_DEBUG=ON ..)
SET(BUILD_DEBUG TRUE)

# 引用公用的 cmake 文件
INCLUDE(app)
