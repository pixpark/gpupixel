# Custom Output and Rendering

This article explains how to implement custom output and rendering functionality by inheriting from the `Sink` base class. GPUPixel provides two typical implementation demo: `SinkRender` and `SinkRawData`, which we will use to illustrate the implementation process.

## Sink Base Class

The `Sink` class is the base class for all output targets, defining the following key functionalities:

- Managing input framebuffers
- Handling rotation modes
- Providing update mechanism

Main interfaces:

```cpp
class Sink {
 public:
  // Constructor, specify input number
  Sink(int inputNumber = 1);
  
  // Set input framebuffer
  virtual void SetInputFramebuffer(std::shared_ptr<GPUPixelFramebuffer> framebuffer,
                                 RotationMode rotation_mode = NoRotation,
                                 int texIdx = 0);
  
  // Update processing
  virtual void Render(){};
};
```

## Implementing Custom Sink

### 1. Basic Steps

1. Inherit from the `Sink` class
2. Implement constructor with necessary initialization
3. Override `SetInputFramebuffer` method (optional)
4. Override `update` method to implement specific rendering logic

### 2. Rendering to Screen - SinkRender

`SinkRender` implements functionality to render images to the screen. Main features:

- Supports multiple fill modes (stretch, preserve aspect ratio, etc.)
- Supports mirror display
- Handles different rotation modes

Key implementation:

```cpp
class SinkRender : public Sink {
 public:
  // Initialize shader program and attribute locations
  void Init() {
 
  }

  // Implement update method for actual rendering
  void Render() override {
     // do render
  }
};
```

### 3. Raw Data Output - SinkRawData

`SinkRawData` implements functionality to output rendering results as raw data. Main features:

- Supports RGBA and I420 format output
- Uses PBO (Pixel Buffer Object) for optimized reading performance
- Data is read via getter APIs after the pipeline has run (pull model, not callbacks)

Public API (call after the source has run `ProcessData` or `Render()`):

```cpp
class SinkRawData : public Sink {
 public:
  static std::shared_ptr<SinkRawData> Create();
  void Render() override;

  // Get output dimensions and buffers (pull model, not callbacks)
  const uint8_t* GetRgbaBuffer();
  const uint8_t* GetI420Buffer();
  int GetWidth() const;
  int GetHeight() const;
};
```

## Implementation Guidelines

1. **Initialization**
   - Initialize basic member variables in constructor
   - Create and configure shader programs
   - Initialize necessary OpenGL resources

2. **Resource Management**
   - Properly release OpenGL resources in destructor
   - Use smart pointers for memory management
   - Consider thread safety

3. **Performance Optimization**
   - Use PBO for asynchronous pixel transfer
   - Avoid frequent memory allocation and deallocation
   - Cache frequently used data and computation results

4. **Error Handling**
   - Check OpenGL errors
   - Validate input parameters
   - Provide appropriate error feedback

## Example Code

Here's a minimal custom Sink implementation example:

```cpp
class MyCustomTarget : public Sink {
 public:
  MyCustomTarget() : Sink(1) {
    // Initialize shaders and other resources
    initShaders();
  }

  ~MyCustomTarget() {
    // Cleanup resources
    cleanup();
  }

  void Render() override {
    if (!IsReady()) return;
    
    // Setup render state
    setupRenderState();
    
    // Execute custom rendering logic
    Render();
    
    // Process output
    processOutput();
  }

 private:
  // Custom implementation details
};
```

## Summary

Implementing custom output and rendering by inheriting from the `Sink` class involves the following process:

1. Create custom class inheriting from `Sink`
2. Implement necessary initialization and cleanup logic
3. Override `update` method to implement rendering logic
4. Implement output processing as needed

By properly utilizing OpenGL features and following best practices, you can achieve efficient and stable custom output targets.