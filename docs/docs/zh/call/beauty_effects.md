---
title: 美颜特效
editLink: true
description: 本文介绍如何在 GPUPixel 库中使用美颜特效滤镜。
---

# 美颜特效

## 高级滤镜链示例

对于更复杂的效果，你可以创建多个滤镜并将它们链接在一起：

```cpp
gpupixel::GPUPixelContext::getInstance()->runSync([&] {
    // 创建源
    gpuPixelRawInput = SourceRawDataInput::create();
    
     // 创建目标
    target_raw_output_ = TargetRawDataOutput::create();
    
    // 创建滤镜
    lipstick_filter_ = LipstickFilter::create();
    blusher_filter_ = BlusherFilter::create();
    face_reshape_filter_ = FaceReshapeFilter::create();
    beauty_face_filter_ = BeautyFaceFilter::create();
    
    // 为需要面部特征的滤镜注册人脸关键点回调
    gpuPixelRawInput->RegLandmarkCallback([=](std::vector<float> landmarks) {
       lipstick_filter_->SetFaceLandmarks(landmarks);
       blusher_filter_->SetFaceLandmarks(landmarks);
       face_reshape_filter_->SetFaceLandmarks(landmarks);
    });
    
    // 链接滤镜链
    gpuPixelRawInput->addTarget(lipstick_filter_)
                     ->addTarget(blusher_filter_)
                     ->addTarget(face_reshape_filter_)
                     ->addTarget(beauty_face_filter_)
                     ->addTarget(target_raw_output_);
});
```

## 调整滤镜参数

每个滤镜都有可以调整的特定参数：

### 美颜滤镜

```cpp
// 设置磨皮程度 (0.0-1.0)
beauty_face_filter_->setBlurAlpha(value/10);

// 设置美白程度 (0.0-1.0)
beauty_face_filter_->setWhite(value/20);
```

### 美型滤镜

```cpp
// 设置瘦脸程度 (0.0-1.0)
face_reshape_filter_->setFaceSlimLevel(value/100);

// 设置大眼程度 (0.0-1.0)
face_reshape_filter_->setEyeZoomLevel(value/50);
```

### 美妆滤镜

```cpp
// 设置口红浓度 (0.0-1.0)
lipstick_filter_->setBlendLevel(value/10);

// 设置腮红浓度 (0.0-1.0)
blusher_filter_->setBlendLevel(value/10);
```