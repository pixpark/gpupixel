#!/bin/bash
# iOS Build Script
set -e  # Exit immediately if a command exits with a non-zero status

echo "===== Starting iOS Build ====="

# Create build directory
mkdir -p build || {
  echo "Error: Cannot create build directory"
  exit 1
}

# Configure project - Using iOS toolchain
echo "Configuring iOS project..."
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=../cmake/ios.toolchain.cmake -DPLATFORM=OS64 -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=output || {
  echo "Error: Project configuration failed"
  exit 2
}

# Build project - Using multi-threaded compilation
echo "Building iOS project..."
cmake --build build --config Release --parallel $(sysctl -n hw.ncpu) || {
  echo "Error: Project build failed"
  exit 3
}

# Install to output directory
echo "Installing to output directory..."
cmake --install build --config Release || {
  echo "Error: Project installation failed"
  exit 4
}

echo "===== iOS Build Complete, Installation Directory: output ====="
exit 0  # Exit normally with return code 0 