# Custom Filter

This guide explains how to create custom filters by inheriting from the `Filter` class in GPUPixel.

## Overview

GPUPixel's filter system is built on OpenGL shaders. Each filter inherits from the base `Filter` class and implements specific image processing effects through shader programs.

## Basic Structure

A typical custom filter class has the following structure:

```cpp
class CustomFilter : public Filter {
 public:
  static std::shared_ptr<CustomFilter> create();
  bool init();
  virtual bool proceed(bool bUpdateTargets = true, int64_t frameTime = 0) override;

  // Custom parameter setters
  void setParameter(float value);

 protected:
  CustomFilter(){};

  // Filter parameters
  float _parameter;
};
```

## Implementation Steps

### 1. Create Factory Method

Implement a static factory method that creates and initializes the filter:

```cpp
std::shared_ptr<CustomFilter> CustomFilter::create() {
  auto ret = std::shared_ptr<CustomFilter>(new CustomFilter());
  if (ret && !ret->init()) {
    ret.reset();
  }
  return ret;
}
```

### 2. Initialize Filter

Implement the `init()` method to set up shader programs and register parameters:

```cpp
bool CustomFilter::init() {
  // Initialize with shader string
  if (!initWithFragmentShaderString(kCustomFragmentShader)) {
    return false;
  }

  // Register parameters
  _parameter = 1.0f;
  registerProperty(
      "parameter",
      _parameter,
      "Description of the parameter",
      [this](float& value) { setParameter(value); });

  return true;
}
```

### 3. Define Shader Program

Create GLSL shader program for your filter effect:

```cpp
const std::string kCustomFragmentShader = R"(
    precision highp float;
    uniform sampler2D inputImageTexture;
    uniform float parameter;
    varying highp vec2 textureCoordinate;

    void main() {
      // Implement your filter effect here
      vec4 textureColor = texture2D(inputImageTexture, textureCoordinate);
      gl_FragColor = textureColor;
    })"
```

### 4. Implement Processing Logic

Override the `proceed()` method to update shader uniforms and process the image:

```cpp
bool CustomFilter::proceed(bool bUpdateTargets, int64_t frameTime) {
  // Update shader uniforms
  _filterProgram->setUniformValue("parameter", _parameter);
  
  // Call base class implementation
  return Filter::proceed(bUpdateTargets, frameTime);
}
```

## Example: HueFilter

Here's a real example of the `HueFilter` that adjusts image hue:

```cpp
class HueFilter : public Filter {
 public:
  static std::shared_ptr<HueFilter> create();
  bool init();
  virtual bool proceed(bool bUpdateTargets = true, int64_t frameTime = 0) override;

  void setHueAdjustment(float hueAdjustment);

 protected:
  HueFilter(){};
  float _hueAdjustment;
};
```

The `HueFilter` demonstrates:
- Parameter registration with callback
- Shader uniform updates in proceed()
- Value conversion in parameter setter

## Best Practices

1. **Parameter Management**
   - Use meaningful parameter names
   - Provide clear parameter descriptions
   - Implement proper value validation

2. **Shader Optimization**
   - Keep shaders simple and efficient
   - Use appropriate precision qualifiers
   - Consider performance impact

3. **Error Handling**
   - Check initialization success
   - Validate parameter values
   - Handle resource cleanup

## See Also

- [Custom Input](./custom_input.md)
- [Custom Target](./custom_target.md)