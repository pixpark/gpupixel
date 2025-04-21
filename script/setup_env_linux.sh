#!/bin/bash

# ========================================
# Script Name: setup_deps.sh
# Function: Force install all development dependencies (without checking existing environment)
# Supported Systems: Ubuntu/Debian/CentOS/RHEL
# ========================================

# Only supports Linux systems
if [[ "$OSTYPE" != "linux-gnu"* ]]; then
    echo "Error: This script only supports Linux systems"
    exit 1
fi

# Detect distribution type
if [ -f /etc/os-release ]; then
    . /etc/os-release
    OS=$ID
elif type lsb_release >/dev/null 2>&1; then
    OS=$(lsb_release -si | tr '[:upper:]' '[:lower:]')
else
    echo "Cannot detect Linux distribution"
    exit 1
fi

# ========================================
# Install all dependencies (without checking existing environment)
# ========================================
install_all_deps() {
    case $OS in
        ubuntu|debian)
            echo "[Ubuntu/Debian] Forcibly installing all development dependencies..."
            sudo apt-get update
            # Install compilation toolchain
            sudo apt-get install -y build-essential
            # Install CMake
            sudo apt-get install -y cmake
            # Install X11/OpenGL dependencies
            sudo apt-get install -y \
                libx11-dev libxrandr-dev libxinerama-dev \
                libxcursor-dev libxi-dev \
                libglu1-mesa-dev freeglut3-dev mesa-common-dev
            ;;

        centos|rhel|fedora|amzn)
            echo "[RedHat Family] Forcibly installing all development dependencies..."
            # Install compilation toolchain
            if command -v dnf >/dev/null 2>&1; then
                sudo dnf groupinstall -y "Development Tools"
                sudo dnf install -y gcc-c++
            elif command -v yum >/dev/null 2>&1; then
                sudo yum groupinstall -y "Development Tools"
                sudo yum install -y gcc-c++
            else
                echo "Package manager yum/dnf not found"
                exit 1
            fi

            # Install CMake
            sudo $PKG_MGR install -y cmake

            # Install X11/OpenGL dependencies
            sudo $PKG_MGR install -y \
                libX11-devel libXrandr-devel libXinerama-devel \
                libXcursor-devel libXi-devel \
                mesa-libGLU-devel freeglut-devel mesa-common-devel
            ;;

        *)
            echo "Unsupported distribution: $OS"
            exit 1
            ;;
    esac
}

# ========================================
# Main Logic
# ========================================
# Determine package manager (RedHat family needs this)
if [[ "$OS" == "centos" || "$OS" == "rhel" || "$OS" == "amzn" ]]; then
    PKG_MGR=$(command -v dnf || command -v yum)
fi

# Force install all dependencies (regardless of existing environment)
install_all_deps

# Verify installation results
echo "Verifying installation results:"
command -v gcc && echo " - GCC is installed: $(gcc --version | head -n1)"
command -v cmake && echo " - CMake is installed: $(cmake --version | head -n1)"
echo "All specified development libraries have been forcibly installed"