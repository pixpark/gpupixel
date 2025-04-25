#!/bin/bash
# Linux Build Script
set -e  # Exit immediately if a command exits with a non-zero status

echo "===== Starting Linux Build ====="

# Set script variables
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_DIR="$( cd "${SCRIPT_DIR}/.." && pwd )"
BUILD_DIR="${PROJECT_DIR}/build/linux"
INSTALL_DIR="${PROJECT_DIR}/output"

# Create build directory
mkdir -p "${BUILD_DIR}" || {
  echo "Error: Cannot create build directory"
  exit 1
}

# Configure project - Release mode
echo "Configuring Linux project..."
cmake -B "${BUILD_DIR}" -S "${PROJECT_DIR}" \
  -DCMAKE_BUILD_TYPE=Release \
  -DGPUPIXEL_BUILD_DESKTOP_DEMO=ON \
  -DCMAKE_INSTALL_PREFIX="${INSTALL_DIR}" || {
  echo "Error: Project configuration failed"
  exit 2
}

# Build project - Using multi-threaded compilation
echo "Building Linux project..."
cmake --build "${BUILD_DIR}" --config Release --parallel $(nproc) || {
  echo "Error: Project build failed"
  exit 3
}

# Install to output directory
echo "Installing to output directory..."
cmake --install "${BUILD_DIR}" --config Release || {
  echo "Error: Project installation failed"
  exit 4
}

echo "===== Linux Build Complete, Installation Directory: ${INSTALL_DIR} ====="
exit 0  # Exit normally with return code 0 