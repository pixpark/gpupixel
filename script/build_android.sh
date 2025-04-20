#!/bin/bash
# Android Gradle构建脚本

echo "===== 开始构建Android项目 ====="

# 进入Android项目目录
cd src/android/java || {
  echo "错误：找不到Android项目目录 src/android/java"
  exit 1
}

# 执行Gradle构建
echo "执行Gradle构建..."
./gradlew :gpupixel:assembleRelease --info

# 检查构建结果
if [ $? -ne 0 ]; then
  echo "Android构建失败"
  exit 1
fi

echo "===== Android构建完成 ====="
echo "构建结果应位于: gpupixel/build/outputs/aar 目录" 