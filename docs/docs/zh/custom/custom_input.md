# 自定义输入

本指南将介绍如何通过继承 GPUPixel 中的 `Source` 类来创建自定义输入源。我们将通过 `SourceRawData` 和 `SourceCamera` 两个示例来探索实现过程。

## 概述

要创建自定义输入源，您需要：
1. 继承 `Source` 类
2. 初始化输入源
3. 处理数据输入
4. 管理帧缓冲区
5. 控制数据流

## 类继承

```cpp
class YourCustomInput : public Source {
 public:
  static std::shared_ptr<YourCustomInput> create();
  // 您的自定义方法
 private:
  // 您的私有成员
};
```

## 关键实现步骤

### 1. 初始化

实现静态创建方法和构造函数：

```cpp
std::shared_ptr<YourCustomInput> YourCustomInput::create() {
  auto input = std::shared_ptr<YourCustomInput>(new YourCustomInput());
  if (input->init()) {
    return input;
  }
  return nullptr;
}
```

### 2. 帧缓冲区管理

为您的输入源创建和管理帧缓冲区：

```cpp
if (!_framebuffer || (_framebuffer->getWidth() != width ||
                      _framebuffer->getHeight() != height)) {
  _framebuffer =
      GPUPixelContext::getInstance()->getFramebufferFactory()->fetchFramebuffer(
          width, height);
}
this->setFramebuffer(_framebuffer, outputRotation);
```

### 3. 数据输入处理

实现方法来处理您的输入数据。例如，处理原始数据：

```cpp
void YourCustomInput::setFrameData(const uint8_t* pixels,
                                   int width,
                                   int height,
                                   int stride) {
  // 处理输入数据
  // 更新帧缓冲区
  // 可选：人脸检测
  if(_face_detector) {
    _face_detector->Detect(pixels, width, height,
                          GPUPIXEL_MODE_FMT_VIDEO,
                          GPUPIXEL_FRAME_TYPE_RGBA8888);
  }
  
  // 更新纹理
  glBindTexture(GL_TEXTURE_2D, this->getFramebuffer()->getTexture());
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
               GL_RGBA, GL_UNSIGNED_BYTE, pixels);
}
```

### 4. 数据流控制

调用 `doRender()` 来通过滤镜链处理帧：

```cpp
Source::doRender(true, timestamp);
```

## 示例：SourceRawData

SourceRawData 演示了如何处理原始像素数据：

1. 支持多种输入格式（RGBA 和 I420）
2. 管理 OpenGL 纹理
3. 实现格式转换的着色器程序

主要特性：
- `processData()`：处理 RGBA 和 YUV 数据
- 不同格式的纹理管理
- YUV 到 RGB 转换的着色器程序

## 示例：SourceCamera

SourceCamera 展示了如何处理相机输入：

1. 平台特定的相机初始化
2. 帧捕获和处理
3. 相机控制（启动、停止、翻转）
4. 旋转和镜像处理

主要特性：
- 相机会话管理
- 帧捕获委托
- 输出旋转处理
- 镜像模式支持

## 最佳实践

1. 内存管理
   - 使用智能指针进行资源管理
   - 正确释放 OpenGL 资源
   - 尽可能重用帧缓冲区

2. 线程安全
   - 使用 GPUPixelContext::runSync 进行 OpenGL 操作
   - 在适当的线程中处理帧捕获

3. 错误处理
   - 检查资源初始化
   - 验证输入参数
   - 处理平台特定错误

4. 性能
   - 重用纹理和帧缓冲区
   - 优化数据转换
   - 最小化上下文切换

## 集成

要使用您的自定义输入源：

```cpp
auto input = YourCustomInput::create();
if (input) {
  auto filter = BeautyFaceFilter::create();
  input->addSink(filter);
  // 设置更多滤镜或输出
}
```

本指南涵盖了自定义输入源的基本实现。对于更具体的要求，请参考提供的示例并根据您的需求进行调整。