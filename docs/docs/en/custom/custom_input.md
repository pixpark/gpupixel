# Custom Input Source

This guide explains how to create a custom input source by inheriting from the `Source` class in GPUPixel. We'll explore the implementation process through two examples: `SourceRawData` and `SourceCamera`.

## Overview

To create a custom input source, you need to:
1. Inherit from the `Source` class
2. Initialize the input source
3. Handle data input
4. Manage framebuffer
5. Control data flow

## Class Inheritance

```cpp
class YourCustomInput : public Source {
 public:
  static std::shared_ptr<YourCustomInput> create();
  // Your custom methods
 private:
  // Your private members
};
```

## Key Implementation Steps

### 1. Initialization

Implement a static create method and constructor:

```cpp
std::shared_ptr<YourCustomInput> YourCustomInput::create() {
  auto input = std::shared_ptr<YourCustomInput>(new YourCustomInput());
  if (input->init()) {
    return input;
  }
  return nullptr;
}
```

### 2. Framebuffer Management

Create and manage framebuffer for your input source:

```cpp
if (!_framebuffer || (_framebuffer->getWidth() != width ||
                      _framebuffer->getHeight() != height)) {
  _framebuffer =
      GPUPixelContext::getInstance()->getFramebufferCache()->fetchFramebuffer(
          width, height);
}
this->setFramebuffer(_framebuffer, outputRotation);
```

### 3. Data Input Processing

Implement methods to handle your input data. For example, with raw data:

```cpp
void YourCustomInput::setFrameData(const uint8_t* pixels,
                                   int width,
                                   int height,
                                   int stride) {
  // Process input data
  // Update framebuffer
  // Optional: Face detection
  if(_face_detector) {
    _face_detector->Detect(pixels, width, height,
                          GPUPIXEL_MODE_FMT_VIDEO,
                          GPUPIXEL_FRAME_TYPE_RGBA8888);
  }
  
  // Update texture
  glBindTexture(GL_TEXTURE_2D, this->getFramebuffer()->getTexture());
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
               GL_RGBA, GL_UNSIGNED_BYTE, pixels);
}
```

### 4. Data Flow Control

Call `doRender()` to process the frame through the filter chain:

```cpp
Source::doRender(true, timestamp);
```

## Example: SourceRawData

SourceRawData demonstrates handling raw pixel data:

1. Supports multiple input formats (RGBA and I420)
2. Manages OpenGL textures
3. Implements shader programs for format conversion

Key features:
- `processData()`: Handle RGBA and YUV data
- Texture management for different formats
- Shader program for YUV to RGB conversion

## Example: SourceCamera

SourceCamera shows how to handle camera input:

1. Platform-specific camera initialization
2. Frame capture and processing
3. Camera control (start, stop, flip)
4. Rotation and mirror handling

Key features:
- Camera session management
- Frame capture delegation
- Output rotation handling
- Mirror mode support

## Best Practices

1. Memory Management
   - Use smart pointers for resource management
   - Release OpenGL resources properly
   - Reuse framebuffers when possible

2. Thread Safety
   - Use GPUPixelContext::runSync for OpenGL operations
   - Handle frame capture in appropriate threads

3. Error Handling
   - Check resource initialization
   - Validate input parameters
   - Handle platform-specific errors

4. Performance
   - Reuse textures and framebuffers
   - Optimize data conversion
   - Minimize context switches

## Integration

To use your custom input source:

```cpp
auto input = YourCustomInput::create();
if (input) {
  auto filter = BeautyFaceFilter::create();
  input->addSink(filter);
  // Setup more filters or outputs
}
```

This guide covers the basic implementation of a custom input source. For more specific requirements, refer to the provided examples and adapt them to your needs.