---
title: 美颜特效
editLink: true
description: 本文介绍如何在 GPUPixel 库中使用美颜特效滤镜。
---

# 美颜特效

## 高级滤镜链示例

对于更复杂的效果，你可以创建多个滤镜并将它们链接在一起：

```cpp
gpupixel::GPUPixelContext::GetInstance()->RunSync([&] {
    // 创建源
    gpuPixelRawInput = SourceRawData::Create();
    
     // 创建目标
    target_raw_output_ = SinkRawData::Create();
    
    // 创建滤镜
    lipstick_filter_ = LipstickFilter::Create();
    blusher_filter_ = BlusherFilter::Create();
    face_reshape_filter_ = FaceReshapeFilter::Create();
    beauty_face_filter_ = BeautyFaceFilter::Create();
    
    // 为需要面部特征的滤镜注册人脸关键点回调
    gpuPixelRawInput->RegLandmarkCallback([=](std::vector<float> landmarks) {
       lipstick_filter_->SetFaceLandmarks(landmarks);
       blusher_filter_->SetFaceLandmarks(landmarks);
       face_reshape_filter_->SetFaceLandmarks(landmarks);
    });
    
    // 链接滤镜链
    gpuPixelRawInput->AddSink(lipstick_filter_)
                     ->AddSink(blusher_filter_)
                     ->AddSink(face_reshape_filter_)
                     ->AddSink(beauty_face_filter_)
                     ->AddSink(target_raw_output_);
});
```

## 调整滤镜参数

每个滤镜都有可以调整的特定参数：

### 美颜滤镜

```cpp
// 设置磨皮程度 (0.0-1.0)
beauty_face_filter_->SetBlurAlpha(value/10);

// 设置美白程度 (0.0-1.0)
beauty_face_filter_->SetWhite(value/20);
```

### 美型滤镜

```cpp
// 设置瘦脸程度 (0.0-1.0)
face_reshape_filter_->SetFaceSlimLevel(value/100);

// 设置大眼程度 (0.0-1.0)
face_reshape_filter_->SetEyeZoomLevel(value/50);
```

### 美妆滤镜

```cpp
// 设置口红浓度 (0.0-1.0)
lipstick_filter_->SetBlendLevel(value/10);

// 设置腮红浓度 (0.0-1.0)
blusher_filter_->SetBlendLevel(value/10);
```