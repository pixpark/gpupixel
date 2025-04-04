#!/bin/bash

# 定义要处理的根目录
SRC_DIR="/Users/admin/repos/github/gpl/src"
DEMO_DIR="/Users/admin/repos/github/gpl/demo"

# clang-format命令，可根据需要修改参数
CLANG_FORMAT="clang-format -i"

# 打印标题
echo "开始格式化代码文件..."

# 格式化src目录下的文件
find "$SRC_DIR" -type f \( -name "*.cc" -o -name "*.h" -o -name "*.java" \) \
     -not -path "*/third_party/*" \
     -exec echo "格式化：{}" \; \
     -exec $CLANG_FORMAT {} \;

# 格式化demo目录下的文件
find "$DEMO_DIR" -type f \( -name "*.cc" -o -name "*.h" -o -name "*.java" \) \
     -not -path "*/third_party/*" \
     -exec echo "格式化：{}" \; \
     -exec $CLANG_FORMAT {} \;

echo "格式化完成！"