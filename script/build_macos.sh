#!/bin/bash
# macOS Build Script
set -e  # Exit immediately if a command exits with a non-zero status

echo "===== Starting macOS Build ====="

# Set script variables
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_DIR="$( cd "${SCRIPT_DIR}/.." && pwd )"
BUILD_DIR="${PROJECT_DIR}/build/macos"
INSTALL_DIR="${PROJECT_DIR}/output"

# Create build directory
mkdir -p "${BUILD_DIR}" || {
  echo "Error: Cannot create build directory"
  exit 1
}

# Detect processor architecture
ARCH=$(uname -m)

# Use absolute path for toolchain
CMAKE_TOOLCHAIN="${PROJECT_DIR}/cmake/ios.toolchain.cmake"

# Choose different configuration based on processor architecture
echo "Configuring macOS project..."
if [ "$ARCH" = "x86_64" ]; then
    echo "Detected Intel processor (x86_64)"
    # Intel processor configuration
    cmake -B "${BUILD_DIR}" -S "${PROJECT_DIR}" \
        -DCMAKE_TOOLCHAIN_FILE="${CMAKE_TOOLCHAIN}" \
        -DPLATFORM=MAC \
        -DCMAKE_BUILD_TYPE=Release \
        -DGPUPIXEL_BUILD_DESKTOP_DEMO=ON \
        -DCMAKE_INSTALL_PREFIX="${INSTALL_DIR}" || {
        echo "Error: Intel platform project configuration failed"
        exit 2
    }
else
    echo "Detected ARM processor (arm64)"
    # ARM processor configuration
    cmake -B "${BUILD_DIR}" -S "${PROJECT_DIR}" \
        -DCMAKE_TOOLCHAIN_FILE="${CMAKE_TOOLCHAIN}" \
        -DPLATFORM=MAC_ARM64 \
        -DCMAKE_BUILD_TYPE=Release \
        -DGPUPIXEL_BUILD_DESKTOP_DEMO=ON \
        -DCMAKE_INSTALL_PREFIX="${INSTALL_DIR}" || {
        echo "Error: ARM platform project configuration failed"
        exit 2
    }
fi

# Build project - Using multi-threaded compilation
echo "Building macOS project..."
cmake --build "${BUILD_DIR}" --config Release --parallel $(sysctl -n hw.ncpu) || {
    echo "Error: Project build failed"
    exit 3
}

# Install to output directory
echo "Installing to output directory..."
cmake --install "${BUILD_DIR}" --config Release || {
    echo "Error: Project installation failed"
    exit 4
}

echo "===== macOS Build Complete, Installation Directory: ${INSTALL_DIR} ====="
exit 0  # Exit normally with return code 0 