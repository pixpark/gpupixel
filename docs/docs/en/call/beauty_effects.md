---
title: Beauty Effects
editLink: true
description: This article introduces how to use beauty effects filters in GPUPixel library.
---

# Beauty Effects

## Advanced Filter Chain Example

For more complex effects, you can create multiple filters and chain them together:

```cpp
gpupixel::GPUPixelContext::getInstance()->runSync([&] {
    // Create source
    gpuPixelRawInput = SourceRawData::create();
    
     // Create sink
    target_raw_output_ = SinkRawData::create();
    
    // Create filters
    lipstick_filter_ = LipstickFilter::create();
    blusher_filter_ = BlusherFilter::create();
    face_reshape_filter_ = FaceReshapeFilter::create();
    beauty_face_filter_ = BeautyFaceFilter::create();
    
    // Register face landmark callback for filters that need facial features
    gpuPixelRawInput->RegLandmarkCallback([=](std::vector<float> landmarks) {
       lipstick_filter_->SetFaceLandmarks(landmarks);
       blusher_filter_->SetFaceLandmarks(landmarks);
       face_reshape_filter_->SetFaceLandmarks(landmarks);
    });
    
    // Link the filter chain
    gpuPixelRawInput->addSink(lipstick_filter_)
                     ->addSink(blusher_filter_)
                     ->addSink(face_reshape_filter_)
                     ->addSink(beauty_face_filter_)
                     ->addSink(target_raw_output_);
});
```

## Adjusting Filter Parameters

Each filter has specific parameters that can be adjusted:

### Beauty Face Filter

```cpp
// Set smoothing level (0.0-1.0)
beauty_face_filter_->setBlurAlpha(value/10);

// Set whitening level (0.0-1.0)
beauty_face_filter_->setWhite(value/20);
```

### Face Reshape Filter

```cpp
// Set face slimming level (0.0-1.0)
face_reshape_filter_->setFaceSlimLevel(value/100);

// Set eye enlargement level (0.0-1.0)
face_reshape_filter_->setEyeZoomLevel(value/50);
```

### Makeup Filters

```cpp
// Set lipstick intensity (0.0-1.0)
lipstick_filter_->setBlendLevel(value/10);

// Set blusher intensity (0.0-1.0)
blusher_filter_->setBlendLevel(value/10);
```