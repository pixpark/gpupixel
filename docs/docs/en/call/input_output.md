---
title: Input and Output
editLink: true
description: This article introduces how to handle input and output data in GPUPixel library.
---

# Input and Output

## Inputting Video/Image Data

GPUPixel supports different input formats:

### RGBA Data Input

`SourceRawData` accepts pixel data and runs the filter chain via `ProcessData`. The last parameter is the pixel format:

```cpp
// For RGBA data (use GPUPIXEL_FRAME_TYPE_RGBA or GPUPIXEL_FRAME_TYPE_BGRA)
source_raw_input_->ProcessData(pixels, width, height, stride,
                              GPUPIXEL_FRAME_TYPE_RGBA);
```

Example with camera frame processing:

```cpp
gpuPixelRawInput->ProcessData(pixels, width, height, stride,
                              GPUPIXEL_FRAME_TYPE_RGBA);
```

Currently `SourceRawData` only supports RGBA/BGRA; convert YUV to RGBA before passing to `ProcessData`.
 

## Getting Output Data

After the pipeline has run (e.g. after calling `ProcessData` or `SourceImage::Render()`), read the result from `SinkRawData`:

```cpp
// Get RGBA result
const uint8_t* rgba_data = target_raw_output_->GetRgbaBuffer();
int width = target_raw_output_->GetWidth();
int height = target_raw_output_->GetHeight();
size_t rgba_size = width * height * 4;
// Use rgba_data for display or further processing

// Get I420 result
const uint8_t* i420_data = target_raw_output_->GetI420Buffer();
size_t y_size = width * height;
const uint8_t* uData = i420_data + y_size;
const uint8_t* vData = i420_data + y_size + y_size / 4;
```