---
title: 美颜特效
editLink: true
description: 本文介绍如何在 GPUPixel 库中使用美颜特效滤镜。
---

# 美颜特效

## 高级滤镜链示例

对于更复杂的效果，你可以创建多个滤镜并将它们链接在一起：

```cpp
   // 创建源
   gpuPixelRawInput = SourceRawData::Create();

   // 创建目标
   target_raw_output_ = SinkRawData::Create();

   // 创建滤镜
   lipstick_filter_ = LipstickFilter::Create();
   blusher_filter_ = BlusherFilter::Create();
   face_reshape_filter_ = FaceReshapeFilter::Create();
   beauty_face_filter_ = BeautyFaceFilter::Create();

   // 创建人脸检测器（需启用 GPUPIXEL_ENABLE_FACE_DETECTOR）
   face_detector_ = FaceDetector::Create();

   // 链接滤镜链
   gpuPixelRawInput->AddSink(lipstick_filter_)
                  ->AddSink(blusher_filter_)
                  ->AddSink(face_reshape_filter_)
                  ->AddSink(beauty_face_filter_)
                  ->AddSink(target_raw_output_);
```

在每帧处理前，用人脸检测器检测关键点并传给需要面部特征的滤镜：

```cpp
   // 获取当前帧图像数据（宽、高、stride 等）
   const uint8_t* buffer = ...;  // 例如从相机或其它输入获取
   int width = ..., height = ..., stride = ...;

   std::vector<float> landmarks = face_detector_->Detect(
       buffer, width, height, stride,
       GPUPIXEL_MODE_FMT_PICTURE, GPUPIXEL_FRAME_TYPE_RGBA);

   if (!landmarks.empty()) {
      lipstick_filter_->SetFaceLandmarks(landmarks);
      blusher_filter_->SetFaceLandmarks(landmarks);
      face_reshape_filter_->SetFaceLandmarks(landmarks);
   }

   // 再执行滤镜链：使用 SourceRawData 时调用 ProcessData 送入本帧并触发渲染；
   // 若使用 SourceImage，则改为先取 buffer = source->GetRgbaImageBuffer() 再 source->Render()
   gpuPixelRawInput->ProcessData(buffer, width, height, stride,
                                GPUPIXEL_FRAME_TYPE_RGBA);
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