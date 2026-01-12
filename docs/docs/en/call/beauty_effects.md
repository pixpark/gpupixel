---
title: Beauty Effects
editLink: true
description: This article introduces how to use beauty effects filters in GPUPixel library.
---

# Beauty Effects

## Advanced Filter Chain Example

For more complex effects, you can create multiple filters and chain them together:

```cpp
   // Create source
   gpuPixelRawInput = SourceRawData::Create();
   
   // Create sink
   target_raw_output_ = SinkRawData::Create();
   
   // Create filters
   lipstick_filter_ = LipstickFilter::Create();
   blusher_filter_ = BlusherFilter::Create();
   face_reshape_filter_ = FaceReshapeFilter::Create();
   beauty_face_filter_ = BeautyFaceFilter::Create();
   
   // Create face detector (requires GPUPIXEL_ENABLE_FACE_DETECTOR)
   face_detector_ = FaceDetector::Create();
   
   // Link the filter chain
   gpuPixelRawInput->AddSink(lipstick_filter_)
                  ->AddSink(blusher_filter_)
                  ->AddSink(face_reshape_filter_)
                  ->AddSink(beauty_face_filter_)
                  ->AddSink(target_raw_output_);
```

Before processing each frame, run face detection and pass the landmarks to filters that need facial features:

```cpp
   // Get current frame image data (width, height, stride, etc.)
   const uint8_t* buffer = ...;  // e.g. from camera or other input
   int width = ..., height = ..., stride = ...;

   std::vector<float> landmarks = face_detector_->Detect(
       buffer, width, height, stride,
       GPUPIXEL_MODE_FMT_PICTURE, GPUPIXEL_FRAME_TYPE_RGBA);

   if (!landmarks.empty()) {
      lipstick_filter_->SetFaceLandmarks(landmarks);
      blusher_filter_->SetFaceLandmarks(landmarks);
      face_reshape_filter_->SetFaceLandmarks(landmarks);
   }

   // Then run the filter pipeline: with SourceRawData call ProcessData to feed
   // the frame and trigger rendering; with SourceImage use GetRgbaImageBuffer()
   // then source->Render()
   gpuPixelRawInput->ProcessData(buffer, width, height, stride,
                                GPUPIXEL_FRAME_TYPE_RGBA);
```

## Adjusting Filter Parameters

Each filter has specific parameters that can be adjusted:

### Beauty Face Filter

```cpp
// Set smoothing level (0.0-1.0)
beauty_face_filter_->SetBlurAlpha(value/10);

// Set whitening level (0.0-1.0)
beauty_face_filter_->SetWhite(value/20);
```

### Face Reshape Filter

```cpp
// Set face slimming level (0.0-1.0)
face_reshape_filter_->SetFaceSlimLevel(value/100);

// Set eye enlargement level (0.0-1.0)
face_reshape_filter_->SetEyeZoomLevel(value/50);
```

### Makeup Filters

```cpp
// Set lipstick intensity (0.0-1.0)
lipstick_filter_->SetBlendLevel(value/10);

// Set blusher intensity (0.0-1.0)
blusher_filter_->SetBlendLevel(value/10);
```