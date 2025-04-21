#!/bin/bash
# Linux Build Script
set -e  # Exit immediately if a command exits with a non-zero status

echo "===== Starting Linux Build ====="

# Create build directory
mkdir -p build || {
  echo "Error: Cannot create build directory"
  exit 1
}

# Configure project - Release mode
echo "Configuring Linux project..."
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release -DGPUPIXEL_BUILD_DESKTOP_DEMO=ON -DCMAKE_INSTALL_PREFIX=output || {
  echo "Error: Project configuration failed"
  exit 2
}

# Build project - Using multi-threaded compilation
echo "Building Linux project..."
cmake --build build --config Release --parallel $(nproc) || {
  echo "Error: Project build failed"
  exit 3
}

# Install to output directory
echo "Installing to output directory..."
cmake --install build --config Release || {
  echo "Error: Project installation failed"
  exit 4
}

echo "===== Linux Build Complete, Installation Directory: output ====="
exit 0  # Exit normally with return code 0 