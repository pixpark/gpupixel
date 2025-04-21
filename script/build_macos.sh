#!/bin/bash
# macOS Build Script
set -e  # Exit immediately if a command exits with a non-zero status

echo "===== Starting macOS Build ====="

# Create build directory
mkdir -p build || {
  echo "Error: Cannot create build directory"
  exit 1
}

# Detect processor architecture
ARCH=$(uname -m)

# Choose different configuration based on processor architecture
echo "Configuring macOS project..."
if [ "$ARCH" = "x86_64" ]; then
    echo "Detected Intel processor (x86_64)"
    # Intel processor configuration
    cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=../cmake/ios.toolchain.cmake -DPLATFORM=MAC -DCMAKE_BUILD_TYPE=Release -DGPUPIXEL_BUILD_DESKTOP_DEMO=ON -DCMAKE_INSTALL_PREFIX=output || {
        echo "Error: Intel platform project configuration failed"
        exit 2
    }
else
    echo "Detected ARM processor (arm64)"
    # ARM processor configuration
    cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=../cmake/ios.toolchain.cmake -DPLATFORM=MAC_ARM64 -DCMAKE_BUILD_TYPE=Release -DGPUPIXEL_BUILD_DESKTOP_DEMO=ON -DCMAKE_INSTALL_PREFIX=output || {
        echo "Error: ARM platform project configuration failed"
        exit 2
    }
fi

# Build project - Using multi-threaded compilation
echo "Building macOS project..."
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

echo "===== macOS Build Complete, Installation Directory: output ====="
exit 0  # Exit normally with return code 0 