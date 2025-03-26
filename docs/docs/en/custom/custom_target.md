# Custom Output and Rendering

This article explains how to implement custom output and rendering functionality by inheriting from the `Sink` base class. GPUPixel provides two typical implementation examples: `SinkRender` and `SinkRawData`, which we will use to illustrate the implementation process.

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
  virtual void setInputFramebuffer(std::shared_ptr<Framebuffer> framebuffer,
                                 RotationMode rotationMode = NoRotation,
                                 int texIdx = 0);
  
  // Update processing
  virtual void render(){};
};
```

## Implementing Custom Sink

### 1. Basic Steps

1. Inherit from the `Sink` class
2. Implement constructor with necessary initialization
3. Override `setInputFramebuffer` method (optional)
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
  void init() {
 
  }

  // Implement update method for actual rendering
  void render() override {
     // do render
  }
};
```

### 3. Raw Data Output - SinkRawData

`SinkRawData` implements functionality to output rendering results as raw data. Main features:

- Supports RGBA and I420 format output
- Uses PBO (Pixel Buffer Object) for optimized reading performance
- Supports asynchronous callbacks

Key implementation:

```cpp
class SinkRawData : public Sink {
 public:
  // Set callback functions
  void setI420Callbck(RawOutputCallback cb);
  void setPixelsCallbck(RawOutputCallback cb);

  // Implement update method
  void render() override {
    // Check input size changes
    if (_width != width || _height != height) {
      initPBO(width, height);
      initFrameBuffer(width, height);
      initOutputBuffer(width, height);
    }
    // Render to output
    renderToOutput();
    // Read pixel data using PBO
    readPixelsWithPBO(_width, _height);
    // Output data through callback
    if (i420_callback_) {
      i420_callback_(_yuvFrameBuffer, _width, _height, _frame_ts);
    }
  }
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

  void render() override {
    if (!isPrepared()) return;
    
    // Setup render state
    setupRenderState();
    
    // Execute custom rendering logic
    render();
    
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