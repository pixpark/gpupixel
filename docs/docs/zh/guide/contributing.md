# 贡献指南

感谢您对 GPUPixel 项目的关注！本指南将帮助您开始参与项目贡献。

## 贡献方式

### 代码贡献

1. **Fork 和克隆**
   - Fork [GPUPixel 仓库](https://github.com/pixpark/gpupixel)
   - 在本地克隆您的 fork 仓库

2. **创建分支**
   - 为您的功能或修复创建新分支
   - 使用清晰、描述性的分支名称

3. **进行修改**
   - 遵循我们的代码风格和规范
   - 编写清晰的提交信息
   - 必要时添加测试

4. **提交 Pull Request**
   - 确保 PR 描述清楚地说明了更改内容
   - 关联相关的 issues

### 文档贡献

- 修复文档中的错别字或优化表述
- 为功能添加缺失的文档
- 将文档翻译成其他语言
- 改进代码示例

### Bug 报告

报告 bug 时：
- 使用[问题模板](https://github.com/pixpark/gpupixel/issues/new/choose)
- 包含清晰的重现步骤
- 提供系统环境信息
- 必要时添加截图

### 功能建议

提出功能建议时：
- 说明您要解决的问题
- 描述功能及其优势
- 考虑实现的复杂度

## 开发环境设置

1. **环境要求**
   - C++11 编译器
   - CMake 3.10+
   - OpenGL/ES 开发环境

2. **构建步骤**

```bash
   git clone https://github.com/YOUR_USERNAME/gpupixel.git
   cd gpupixel
   mkdir build && cd build
   cmake ..
   make
```

## 代码风格指南

我们遵循[谷歌 C++ 风格指南](https://google.github.io/styleguide/cppguide.html)并做了一些调整：

### 基本规则
- 合理使用 C++11 特性
- 遵循现有代码格式
- 为复杂逻辑添加注释
- 保持函数专注和简洁

### 具体规范
- 使用 4 个空格进行缩进，不使用制表符
- 每行最大长度为 100 个字符
- 变量和函数名使用 `camelCase` 风格
- 类名使用 `PascalCase` 风格
- 常量和宏使用 `UPPER_CASE` 风格
- 左大括号放在同一行
- 运算符和逗号后要加空格

### 示例
```cpp
class MyClass {
public:
    void doSomething(const std::string& inputParam) {
        if (inputParam.empty()) {
            return;
        }
        
        int someValue = calculateValue(inputParam);
        processResult(someValue);
    }
    
private:
    static const int MAX_VALUE = 100;
    std::string className_;
};
```

## 测试
- 确保现有测试通过
- 尽可能在多个平台上测试
## 社区指南
- 保持尊重和包容
- 在讨论中帮助他人
- 遵守我们的行为准则
- 适当给予信任
## 获取帮助
- 在 GitHub Discussions 中提问
- 通过 电子邮件 联系维护者
## 许可证
通过为 GPUPixel 做出贡献，您同意您的贡献将在 Apache-2.0 License 下授权。