#!/bin/bash

# 定义要处理的根目录
SRC_DIR="$(pwd)/src"
DEMO_DIR="$(pwd)/demo"
 
CLANG_FORMAT="clang-format -i"
 
echo "start formatting..."
 
find "$SRC_DIR" -type f \( -name "*.cc" -o -name "*.h" -o -name "*.mm" -o -name "*.java" \) \
     -not -path "*/third_party/*" \
     -exec echo "格式化：{}" \; \
     -exec $CLANG_FORMAT {} \;

# format demo
find "$DEMO_DIR" -type f \( -name "*.cc" -o -name "*.h" -o -name "*.mm" -o -name "*.java" \) \
     -not -path "*/third_party/*" \
     -exec echo "格式化：{}" \; \
     -exec $CLANG_FORMAT {} \;

echo "done!"