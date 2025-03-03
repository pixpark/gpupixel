---
title: 基础滤镜
editLink: true
description: 本文介绍如何在 GPUPixel 库中使用基础滤镜。
---

# 基础滤镜

## 基本概念

GPUPixel 采用滤镜链架构设计，视频或图像数据通过一系列滤镜流动以实现各种效果。主要组件包括：

- **Source**：视频/图像数据的输入（例如 `SourceRawDataInput`）
- **Filters**：应用效果的处理单元（例如 `BeautyFaceFilter`、`FaceReshapeFilter`）
- **Target**：输出目标（例如 `TargetRawDataOutput`）

## 初始化

在使用 GPUPixel 之前，你需要初始化上下文并创建滤镜链：

```cpp
// 初始化 GPUPixel 上下文并在其线程中运行操作
gpupixel::GPUPixelContext::getInstance()->runSync([&] {
    // 在这里创建你的滤镜并设置滤镜链
});
```

## 创建滤镜链

典型的滤镜链设置包括创建源、滤镜和目标，然后将它们链接在一起：

```cpp
// 声明组件
std::shared_ptr<SourceRawDataInput> source_raw_input_;
std::shared_ptr<BeautyFaceFilter> beauty_face_filter_;
std::shared_ptr<TargetRawDataOutput> target_raw_output_;

// 初始化并链接组件
gpupixel::GPUPixelContext::getInstance()->runSync([&] {
    // 创建源
    source_raw_input_ = SourceRawDataInput::create();
    
    // 创建滤镜
    beauty_face_filter_ = BeautyFaceFilter::create();
    
    // 创建目标
    target_raw_output_ = TargetRawDataOutput::create();
    
    // 链接滤镜链
    source_raw_input_->addTarget(beauty_face_filter_)
                     ->addTarget(target_raw_output_);
});
```

## 清理

当你使用完 GPUPixel 后，应该清理资源：

```cpp
// 清理 GPUPixel 资源
gpupixel::GPUPixelContext::getInstance()->runSync([&] {
    // 释放你的滤镜引用
    source_raw_input_.reset();
    beauty_face_filter_.reset();
    target_raw_output_.reset();
    // 其他滤镜...
});
```