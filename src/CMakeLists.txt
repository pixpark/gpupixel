# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# 设置 cmkae 最低版本
CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

# 设置 C++ 标准版本为 C++17
set(CMAKE_CXX_STANDARD 17)

# 设置引入的 cmake 文件路径为当前目录
SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")

# 定义当前模块的名称
SET(PROJECT_NAME "gpupixel")
PROJECT(${PROJECT_NAME})

# 定义导出 API 的宏定义(Windows下才有意义)
ADD_DEFINITIONS(-DMYMATH_EXPORT_LIBRARY)

# 引用公用的 cmake 文件
INCLUDE(lib)
