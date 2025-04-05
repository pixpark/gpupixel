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
   
   // Register face landmark callback for filters that need facial features
   gpuPixelRawInput->RegLandmarkCallback([=](std::vector<float> landmarks) {
      lipstick_filter_->SetFaceLandmarks(landmarks);
      blusher_filter_->SetFaceLandmarks(landmarks);
      face_reshape_filter_->SetFaceLandmarks(landmarks);
   });
   
   // Link the filter chain
   gpuPixelRawInput->AddSink(lipstick_filter_)
                  ->AddSink(blusher_filter_)
                  ->AddSink(face_reshape_filter_)
                  ->AddSink(beauty_face_filter_)
                  ->AddSink(target_raw_output_);
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