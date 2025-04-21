#!/bin/bash

# Define root directories to process
SRC_DIR="$(pwd)/src"
DEMO_DIR="$(pwd)/demo"
 
CLANG_FORMAT="clang-format -i"
 
echo "Start formatting..."
 
find "$SRC_DIR" -type f \( -name "*.cc" -o -name "*.h" -o -name "*.mm" -o -name "*.java" \) \
     -not -path "*/third_party/*" \
     -exec echo "Formatting: {}" \; \
     -exec $CLANG_FORMAT {} \;

# Format demo
find "$DEMO_DIR" -type f \( -name "*.cc" -o -name "*.h" -o -name "*.mm" -o -name "*.java" \) \
     -not -path "*/third_party/*" \
     -exec echo "Formatting: {}" \; \
     -exec $CLANG_FORMAT {} \;

# Format cmake
if command -v cmake-format &> /dev/null; then
    echo "Formatting CMake files..."
    cmake-format -i CMakeLists.txt
    cmake-format -i src/CMakeLists.txt
    cmake-format -i third_party/CMakeLists.txt
    cmake-format -i demo/CMakeLists.txt
else
    echo "Skipping CMake formatting: cmake-format not found"
fi

echo "Done!"