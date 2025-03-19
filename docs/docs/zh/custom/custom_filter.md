# 自定义滤镜

本指南介绍如何通过继承 GPUPixel 中的 `Filter` 类来创建自定义滤镜。

## 概述

GPUPixel 的滤镜系统基于 OpenGL 着色器构建。每个滤镜都继承自基础 `Filter` 类，并通过着色器程序实现特定的图像处理效果。

## 基本结构

一个典型的自定义滤镜类具有以下结构：

```cpp
class CustomFilter : public Filter {
 public:
  static std::shared_ptr<CustomFilter> create();
  bool init();
  virtual bool proceed(bool bUpdateTargets = true, int64_t frameTime = 0) override;

  // 自定义参数设置器
  void setParameter(float value);

 protected:
  CustomFilter(){};

  // 滤镜参数
  float _parameter;
};
```

## 实现步骤

### 1. 创建工厂方法

实现一个静态工厂方法来创建和初始化滤镜：

```cpp
std::shared_ptr<CustomFilter> CustomFilter::create() {
  auto ret = std::shared_ptr<CustomFilter>(new CustomFilter());
  if (ret && !ret->init()) {
    ret.reset();
  }
  return ret;
}
```

### 2. 初始化滤镜

实现 `init()` 方法来设置着色器程序和注册参数：

```cpp
bool CustomFilter::init() {
  // 使用着色器字符串初始化
  if (!initWithFragmentShaderString(kCustomFragmentShader)) {
    return false;
  }

  // 注册参数
  _parameter = 1.0f;
  registerProperty(
      "parameter",
      _parameter,
      "参数描述",
      [this](float& value) { setParameter(value); });

  return true;
}
```

### 3. 定义着色器程序

为滤镜效果创建 GLSL 着色器程序：

```cpp
const std::string kCustomFragmentShader = R"(
    precision highp float;
    uniform sampler2D inputImageTexture;
    uniform float parameter;
    varying highp vec2 textureCoordinate;

    void main() {
      // 在此实现滤镜效果
      vec4 textureColor = texture2D(inputImageTexture, textureCoordinate);
      gl_FragColor = textureColor;
    })"
```

### 4. 实现处理逻辑

重写 `proceed()` 方法来更新着色器 uniform 变量并处理图像：

```cpp
bool CustomFilter::proceed(bool bUpdateTargets, int64_t frameTime) {
  // 更新着色器 uniform 变量
  _filterProgram->setUniformValue("parameter", _parameter);
  
  // 调用基类实现
  return Filter::proceed(bUpdateTargets, frameTime);
}
```

## 示例：HueFilter

这是一个调整图像色相的 `HueFilter` 实际示例：

```cpp
class HueFilter : public Filter {
 public:
  static std::shared_ptr<HueFilter> create();
  bool init();
  virtual bool proceed(bool bUpdateTargets = true, int64_t frameTime = 0) override;

  void setHueAdjustment(float hueAdjustment);

 protected:
  HueFilter(){};
  float _hueAdjustment;
};
```

`HueFilter` 展示了：
- 带回调的参数注册
- proceed() 中的着色器 uniform 更新
- 参数设置器中的值转换

## 最佳实践

1. **参数管理**
   - 使用有意义的参数名称
   - 提供清晰的参数描述
   - 实现适当的值验证

2. **着色器优化**
   - 保持着色器简单高效
   - 使用适当的精度限定符
   - 考虑性能影响

3. **错误处理**
   - 检查初始化是否成功
   - 验证参数值
   - 处理资源清理

## 参见

- [自定义输入](./custom_input.md)
- [自定义输出](./custom_target.md)