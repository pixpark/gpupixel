# 自定义Target输出和渲染

本文将介绍如何通过继承 `Target` 基类来实现自定义的输出和渲染功能。GPUPixel 提供了两个典型的实现示例：`TargetView` 和 `TargetRawDataOutput`，我们将通过这两个示例来说明实现流程。

## Target 基类

`Target` 类是所有输出目标的基类，它定义了以下关键功能：

- 管理输入帧缓冲区（Framebuffer）
- 处理旋转模式
- 提供更新机制

主要接口：

```cpp
class Target {
 public:
  // 构造函数，指定输入数量
  Target(int inputNumber = 1);
  
  // 设置输入帧缓冲区
  virtual void setInputFramebuffer(std::shared_ptr<Framebuffer> framebuffer,
                                 RotationMode rotationMode = NoRotation,
                                 int texIdx = 0);
  
  // 更新处理
  virtual void update(int64_t frameTime){};
};
```

## 实现自定义Target

### 1. 基本步骤

1. 继承 `Target` 类
2. 实现构造函数，进行必要的初始化
3. 重写 `setInputFramebuffer` 方法（可选）
4. 重写 `update` 方法，实现具体的渲染逻辑

### 2. 渲染到屏幕 - TargetView

`TargetView` 实现了将图像渲染到屏幕的功能。主要特点：

- 支持多种填充模式（拉伸、保持宽高比等）
- 支持镜像显示
- 处理不同的旋转模式

关键实现：

```cpp
class TargetView : public Target {
 public:
  // 初始化着色器程序和属性位置
  void init() {
   
  }

  // 实现更新方法，执行实际的渲染
  void update(int64_t frameTime) override {
     // do render
  }
};
```

### 3. 原始数据输出 - TargetRawDataOutput

`TargetRawDataOutput` 实现了将渲染结果输出为原始数据的功能。主要特点：

- 支持 RGBA 和 I420 格式输出
- 使用 PBO（Pixel Buffer Object）优化读取性能
- 支持异步回调

关键实现：

```cpp
class TargetRawDataOutput : public Target {
 public:
  // 设置回调函数
  void setI420Callbck(RawOutputCallback cb);
  void setPixelsCallbck(RawOutputCallback cb);

  // 实现更新方法
  void update(int64_t frameTime) override {
    // 检查输入尺寸变化
    if (_width != width || _height != height) {
      initPBO(width, height);
      initFrameBuffer(width, height);
      initOutputBuffer(width, height);
    }
    // 渲染到帧缓冲区
    renderToOutput();
    // 使用 PBO 读取像素数据
    readPixelsWithPBO(_width, _height);
    // 通过回调函数输出数据
    if (i420_callback_) {
      i420_callback_(_yuvFrameBuffer, _width, _height, _frame_ts);
    }
  }
};
```

## 实现建议

1. **初始化**
   - 在构造函数中初始化基本成员变量
   - 创建和配置着色器程序
   - 初始化必要的 OpenGL 资源

2. **资源管理**
   - 在析构函数中正确释放 OpenGL 资源
   - 使用智能指针管理动态分配的内存
   - 注意线程安全性

3. **性能优化**
   - 使用 PBO 进行异步像素传输
   - 避免频繁的内存分配和释放
   - 缓存经常使用的数据和计算结果

4. **错误处理**
   - 检查 OpenGL 错误
   - 验证输入参数
   - 提供适当的错误反馈

## 示例代码

这里是一个最小化的自定义 Target 实现示例：

```cpp
class MyCustomTarget : public Target {
 public:
  MyCustomTarget() : Target(1) {
    // 初始化着色器和其他资源
    initShaders();
  }

  ~MyCustomTarget() {
    // 清理资源
    cleanup();
  }

  void update(int64_t frameTime) override {
    if (!isPrepared()) return;
    
    // 设置渲染状态
    setupRenderState();
    
    // 执行自定义渲染逻辑
    render();
    
    // 处理输出
    processOutput();
  }

 private:
  // 自定义实现细节
};
```

## 总结

继承 `Target` 类实现自定义输出和渲染主要涉及以下流程：

1. 创建自定义类并继承 `Target`
2. 实现必要的初始化和清理逻辑
3. 重写 `update` 方法实现渲染逻辑
4. 根据需要实现输出处理

通过合理使用 OpenGL 功能和遵循最佳实践，可以实现高效和稳定的自定义输出目标。