---
title: Input and Output
editLink: true
description: This article introduces how to handle input and output data in GPUPixel library.
---

# Input and Output

## Inputting Video/Image Data

GPUPixel supports different input formats:

### RGBA Data Input

```cpp
// For RGBA data
source_raw_input_->ProcessData(pixels, width, height, stride);
```

Example with camera frame processing:

```cpp
gpuPixelRawInput->ProcessData(pixels, width, height, stride);
```

### YUV Data Input

```cpp
// For YUV420P data
source_raw_input_->ProcessData(width, height, 
                              bufferY, strideY, 
                              bufferU, strideU,
                              bufferV, strideV);
```
 

## Setting Output Callbacks

You can set callbacks to receive processed data:

```cpp
// RGBA callback
target_raw_output_->setPixelsCallbck([=](const uint8_t *data, 
                                      int width, 
                                      int height, 
                                      int64_t ts) {
    // Process RGBA data
    size_t rgba_size = width * height * 4;
    // Do something with the data
});

// I420 callback
target_raw_output_->setI420Callbck([=](const uint8_t *data, 
                                    int width, 
                                    int height, 
                                    int64_t ts) {
    // Process I420 data
    size_t y_size = width * height;
    const uint8_t *uData = data + y_size;
    const uint8_t *vData = data + y_size + y_size / 4;
    // Do something with the data
});
```