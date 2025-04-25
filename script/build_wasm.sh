#!/bin/bash
# =============================================================================
# GPUPixel WebAssembly Build Script
# 
# This script builds the GPUPixel library for WebAssembly (WASM) platform
# using Emscripten SDK. Before running this script, ensure that emscripten is 
# properly installed, either via emsdk or Homebrew.
# =============================================================================

set -e

# Set script variables
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_DIR="$( cd "${SCRIPT_DIR}/.." && pwd )"
BUILD_DIR="${PROJECT_DIR}/build/wasm"
INSTALL_DIR="${PROJECT_DIR}/output"

# Check if Emscripten is installed
if ! command -v emcc &> /dev/null; then
    # Try to find Homebrew installed emscripten
    if command -v brew &> /dev/null; then
        echo "Detected Homebrew, searching for emscripten..."
        BREW_EMSCRIPTEN=$(brew --prefix emscripten 2>/dev/null || echo "")
        
        if [ -n "$BREW_EMSCRIPTEN" ] && [ -d "$BREW_EMSCRIPTEN" ]; then
            echo "Found Homebrew installed emscripten: $BREW_EMSCRIPTEN"
            export PATH="$BREW_EMSCRIPTEN/bin:$PATH"
            export EMSDK="$BREW_EMSCRIPTEN"
        else
            echo "Homebrew installed emscripten not found, please install first:"
            echo "brew install emscripten"
            exit 1
        fi
    else
        echo "Error: emcc command not found, please install Emscripten."
        echo "  Option 1: Install and activate emsdk (https://emscripten.org/docs/getting_started/downloads.html)"
        echo "  Option 2: Install via Homebrew (brew install emscripten)"
        exit 1
    fi
fi

# Confirm emcc is now available
if ! command -v emcc &> /dev/null; then
    echo "Error: emcc command is still not available even after attempting to locate emscripten."
    exit 1
fi

# Create build directory
mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"

# Build configuration
echo "Configuring WebAssembly build..."
emcmake cmake "${PROJECT_DIR}" \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX="${INSTALL_DIR}" \
  -DGPUPIXEL_BUILD_SHARED_LIBS=OFF \
  -DGPUPIXEL_ENABLE_FACE_DETECTOR=OFF \
  -DGPUPIXEL_BUILD_DESKTOP_DEMO=ON

# Build
echo "Building..."
emmake make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 2)

# Install
echo "Installing..."
emmake make install

echo "WebAssembly build completed!"
echo "Static libraries and demo programs have been installed to: ${INSTALL_DIR}"