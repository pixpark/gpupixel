---
title: 输入和输出
editLink: true
description: 本文介绍如何在 GPUPixel 库中处理输入和输出数据。
---

# 输入和输出

## 视频/图像数据输入

GPUPixel 支持不同的输入格式：

### RGBA 数据输入

```cpp
// 对于 RGBA 数据
source_raw_input_->uploadBytes(pixels, width, height, stride);
```

相机帧处理示例：

```cpp
gpuPixelRawInput->uploadBytes(pixels, width, height, stride);
```

### YUV 数据输入

```cpp
// 对于 YUV420P 数据
source_raw_input_->uploadBytes(width, height, 
                              bufferY, strideY, 
                              bufferU, strideU,
                              bufferV, strideV);
```
 

## 设置输出回调

你可以设置回调来接收处理后的数据：

```cpp
// RGBA 回调
target_raw_output_->setPixelsCallbck([=](const uint8_t *data, 
                                      int width, 
                                      int height, 
                                      int64_t ts) {
    // 处理 RGBA 数据
    size_t rgba_size = width * height * 4;
    // 对数据进行处理
});

// I420 回调
target_raw_output_->setI420Callbck([=](const uint8_t *data, 
                                    int width, 
                                    int height, 
                                    int64_t ts) {
    // 处理 I420 数据
    size_t y_size = width * height;
    const uint8_t *uData = data + y_size;
    const uint8_t *vData = data + y_size + y_size / 4;
    // 对数据进行处理
});
```