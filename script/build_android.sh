#!/bin/bash
# Android Gradle Build Script
set -e  # Exit immediately if a command exits with a non-zero status

# 初始化专业版标志
PRO_VERSION="OFF"

# 解析命令行参数
for arg in "$@"; do
  if [ "$arg" = "--pro" ]; then
    PRO_VERSION="ON"
    echo "Building Professional Version"
  fi
done

echo "===== Starting Android Build ====="

# Enter Android project directory
cd src/android/java || {
  echo "Error: Android project directory src/android/java not found"
  exit 1
}

# Check if gradlew exists
if [ ! -f "./gradlew" ]; then
  echo "Error: gradlew script not found"
  exit 2
fi

# Ensure gradlew has execution permission
chmod +x ./gradlew || {
  echo "Error: Cannot grant execution permission to gradlew"
  exit 3
}

# 添加专业版参数传递到Gradle
if [ "$PRO_VERSION" = "ON" ]; then
  PRO_FLAG="-PGPUPIXEL_PRO_VERSION=true"
else
  PRO_FLAG=""
fi

# Execute Gradle build
echo "Running Gradle build..."
./gradlew :gpupixel:assembleRelease $PRO_FLAG --info || {
  echo "Error: Gradle build failed"
  exit 4
}

# Check build output
if [ ! -d "gpupixel/build/outputs/aar" ]; then
  echo "Error: Build output directory not found"
  exit 5
fi

echo "===== Android Build Complete ====="
echo "Build results located at: gpupixel/build/outputs/aar directory"
exit 0  # Exit normally with return code 0 