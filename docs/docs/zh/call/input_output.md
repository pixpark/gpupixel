---
title: 输入和输出
editLink: true
description: 本文介绍如何在 GPUPixel 库中处理输入和输出数据。
---

# 输入和输出

## 视频/图像数据输入

GPUPixel 支持不同的输入格式：

### RGBA 数据输入

`SourceRawData` 通过 `ProcessData` 送入像素数据并触发滤镜链，最后一个参数为像素格式：

```cpp
// 对于 RGBA 数据（需指定 GPUPIXEL_FRAME_TYPE_RGBA 或 GPUPIXEL_FRAME_TYPE_BGRA）
source_raw_input_->ProcessData(pixels, width, height, stride,
                              GPUPIXEL_FRAME_TYPE_RGBA);
```

相机帧处理示例：

```cpp
gpuPixelRawInput->ProcessData(pixels, width, height, stride,
                              GPUPIXEL_FRAME_TYPE_RGBA);
```

当前 `SourceRawData` 仅支持 RGBA/BGRA 格式，YUV 数据需先转换为 RGBA 再传入。
 

## 获取输出数据

管线执行完成后（例如已调用 `ProcessData` 或 `SourceImage::Render()`），可从 `SinkRawData` 拉取结果：

```cpp
// 获取 RGBA 结果
const uint8_t* rgba_data = target_raw_output_->GetRgbaBuffer();
int width = target_raw_output_->GetWidth();
int height = target_raw_output_->GetHeight();
size_t rgba_size = width * height * 4;
// 使用 rgba_data 进行处理或显示

// 获取 I420 结果
const uint8_t* i420_data = target_raw_output_->GetI420Buffer();
size_t y_size = width * height;
const uint8_t* uData = i420_data + y_size;
const uint8_t* vData = i420_data + y_size + y_size / 4;
```