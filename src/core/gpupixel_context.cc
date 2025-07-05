/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "core/gpupixel_context.h"
#include "utils/dispatch_queue.h"
#include "utils/logging.h"
#include "utils/util.h"
#if defined(GPUPIXEL_WASM)
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

namespace gpupixel {

GPUPixelContext* GPUPixelContext::instance_ = 0;
std::mutex GPUPixelContext::mutex_;

GPUPixelContext::GPUPixelContext() : current_shader_program_(0) {
  FB_LOG_DEBUG("Creating GPUPixelContext");
#if !defined(GPUPIXEL_WASM)
  task_queue_ = std::make_shared<DispatchQueue>();
#endif
  framebuffer_factory_ = new FramebufferFactory();
  Init();
}

GPUPixelContext::~GPUPixelContext() {
  FB_LOG_DEBUG("Destroying GPUPixelContext");
  ReleaseContext();
  delete framebuffer_factory_;
  task_queue_->stop();
}

GPUPixelContext* GPUPixelContext::GetInstance() {
  if (!instance_) {
    std::unique_lock<std::mutex> lock(mutex_);
    if (!instance_) {
      instance_ = new (std::nothrow) GPUPixelContext;
    }
  }
  return instance_;
};

void GPUPixelContext::Destroy() {
  if (instance_) {
    delete instance_;
    instance_ = 0;
  }
}

void GPUPixelContext::Init() {
  SyncRunWithContext([=] {
    FB_LOG_INFO("Initializing GPUPixelContext");
    this->CreateContext();
  });
}

FramebufferFactory* GPUPixelContext::GetFramebufferFactory() const {
  return framebuffer_factory_;
}

void GPUPixelContext::SetActiveGlProgram(GPUPixelGLProgram* shaderProgram) {
  if (current_shader_program_ != shaderProgram) {
    current_shader_program_ = shaderProgram;
    shaderProgram->UseProgram();
  }
}

void GPUPixelContext::Clean() {
  FB_LOG_DEBUG("Cleaning GPUPixelContext resources");
  framebuffer_factory_->Clean();
}

void GPUPixelContext::CreateContext() {
#if defined(GPUPIXEL_IOS)
  FB_LOG_DEBUG("Creating iOS OpenGL ES 2.0 context");
  egl_context_ = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
  if (!egl_context_) {
    FB_LOG_ERROR("Failed to create iOS OpenGL ES 2.0 context");
    return;
  }
  [EAGLContext setCurrentContext:egl_context_];
  FB_LOG_INFO("iOS OpenGL ES 2.0 context created successfully");
#elif defined(GPUPIXEL_MAC)
  FB_LOG_DEBUG("Creating macOS OpenGL context");
  NSOpenGLPixelFormatAttribute pixelFormatAttributes[] = {
      NSOpenGLPFADoubleBuffer,
      NSOpenGLPFAOpenGLProfile,
      NSOpenGLProfileVersionLegacy,
      NSOpenGLPFAAccelerated,
      0,
      NSOpenGLPFAColorSize,
      24,
      NSOpenGLPFAAlphaSize,
      8,
      NSOpenGLPFADepthSize,
      24,
      0};

  pixel_format_ =
      [[NSOpenGLPixelFormat alloc] initWithAttributes:pixelFormatAttributes];
  if (!pixel_format_) {
    FB_LOG_ERROR("Failed to create NSOpenGLPixelFormat");
    return;
  }

  image_processing_context_ =
      [[NSOpenGLContext alloc] initWithFormat:pixel_format_ shareContext:nil];
  if (!image_processing_context_) {
    FB_LOG_ERROR("Failed to create NSOpenGLContext");
    return;
  }

  GLint interval = 0;
  [image_processing_context_ makeCurrentContext];
  [image_processing_context_ setValues:&interval
                          forParameter:NSOpenGLContextParameterSwapInterval];
  FB_LOG_INFO("macOS OpenGL context created successfully");
#elif defined(GPUPIXEL_ANDROID)
  FB_LOG_DEBUG("Creating Android EGL context");
  // Initialize EGL
  egl_display_ = eglGetDisplay(EGL_DEFAULT_DISPLAY);
  if (egl_display_ == EGL_NO_DISPLAY) {
    FB_LOG_ERROR("Failed to get EGL display");
    return;
  }

  EGLint major, minor;
  if (!eglInitialize(egl_display_, &major, &minor)) {
    FB_LOG_ERROR("Failed to initialize EGL");
    return;
  }
  FB_LOG_DEBUG("EGL initialized: version major:{} minor:{}", major, minor);

  // Configure EGL
  const EGLint configAttribs[] = {EGL_RED_SIZE,
                                  8,
                                  EGL_GREEN_SIZE,
                                  8,
                                  EGL_BLUE_SIZE,
                                  8,
                                  EGL_ALPHA_SIZE,
                                  8,
                                  EGL_DEPTH_SIZE,
                                  16,
                                  EGL_STENCIL_SIZE,
                                  0,
                                  EGL_SURFACE_TYPE,
                                  EGL_PBUFFER_BIT,
                                  EGL_RENDERABLE_TYPE,
                                  EGL_OPENGL_ES2_BIT,
                                  EGL_NONE};

  EGLint numConfigs;
  if (!eglChooseConfig(egl_display_, configAttribs, &egl_config_, 1,
                       &numConfigs)) {
    FB_LOG_ERROR("Failed to choose EGL config");
    return;
  }

  // Create EGL context
  const EGLint contextAttribs[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};

  egl_context_ = eglCreateContext(egl_display_, egl_config_, EGL_NO_CONTEXT,
                                  contextAttribs);
  if (egl_context_ == EGL_NO_CONTEXT) {
    FB_LOG_ERROR("Failed to create EGL context");
    return;
  }

  // Create offscreen rendering surface
  const EGLint pbufferAttribs[] = {EGL_WIDTH, 1, EGL_HEIGHT, 1, EGL_NONE};

  egl_surface_ =
      eglCreatePbufferSurface(egl_display_, egl_config_, pbufferAttribs);
  if (egl_surface_ == EGL_NO_SURFACE) {
    FB_LOG_ERROR("Failed to create EGL surface");
    return;
  }

  // Set current context
  if (!eglMakeCurrent(egl_display_, egl_surface_, egl_surface_, egl_context_)) {
    FB_LOG_ERROR("Failed to make EGL context current");
    return;
  }
  FB_LOG_INFO("Android EGL context created successfully");
#elif defined(GPUPIXEL_WIN) || defined(GPUPIXEL_LINUX)
  FB_LOG_DEBUG("Creating Windows/Linux OpenGL context");
  int ret = glfwInit();

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

  if (ret) {
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
  } else {
    FB_LOG_ERROR("Failed to initialize GLFW");
    return;
  }
  gl_context_ = glfwCreateWindow(1, 1, "gpupixel opengl context", NULL, NULL);
  if (!gl_context_) {
    FB_LOG_ERROR("Failed to create GLFW window");
    glfwTerminate();
    return;
  }
  glfwMakeContextCurrent(gl_context_);

  if (!gladLoadGL()) {
    FB_LOG_ERROR("Failed to initialize GLAD");
    return;
  }
  FB_LOG_INFO("Windows/Linux OpenGL context created successfully");
#elif defined(GPUPIXEL_WASM)
  FB_LOG_DEBUG("Creating WebGL context");
  EmscriptenWebGLContextAttributes attrs;
  emscripten_webgl_init_context_attributes(&attrs);
  attrs.majorVersion = 2;  // Use WebGL 2.0
  attrs.minorVersion = 0;
  wasm_context_ = emscripten_webgl_create_context("#gpupixel_canvas", &attrs);
  if (wasm_context_ <= 0) {
    FB_LOG_ERROR("Failed to create WebGL context: {}", wasm_context_);
    return;
  }
  emscripten_webgl_make_context_current(wasm_context_);
  FB_LOG_INFO("WebGL context created successfully");
#endif
}

void GPUPixelContext::UseAsCurrent() {
#if defined(GPUPIXEL_IOS)
  if ([EAGLContext currentContext] != egl_context_) {
    FB_LOG_TRACE("Setting current EAGLContext");
    [EAGLContext setCurrentContext:egl_context_];
  }
#elif defined(GPUPIXEL_MAC)
  if ([NSOpenGLContext currentContext] != image_processing_context_) {
    FB_LOG_TRACE("Setting current NSOpenGLContext");
    [image_processing_context_ makeCurrentContext];
  }
#elif defined(GPUPIXEL_ANDROID)
  if (eglGetCurrentContext() != egl_context_) {
    FB_LOG_TRACE("Setting current EGL context");
    eglMakeCurrent(egl_display_, egl_surface_, egl_surface_, egl_context_);
  }
#elif defined(GPUPIXEL_WIN) || defined(GPUPIXEL_LINUX)
  if (glfwGetCurrentContext() != gl_context_) {
    FB_LOG_TRACE("Setting current GLFW context");
    glfwMakeContextCurrent(gl_context_);
  }
#elif defined(GPUPIXEL_WASM)
  FB_LOG_TRACE("Setting current WebGL context");
  emscripten_webgl_make_context_current(wasm_context_);
#endif
}

void GPUPixelContext::PresentBufferForDisplay() {
#if defined(GPUPIXEL_IOS)
  [egl_context_ presentRenderbuffer:GL_RENDERBUFFER];
#elif defined(GPUPIXEL_MAC)
  // No implementation needed
#elif defined(GPUPIXEL_ANDROID)
  // For offscreen rendering, no need to swap buffers
  // If display to screen is needed, use eglSwapBuffers(egl_display_,
  // egl_surface_);
#endif
}

void GPUPixelContext::ReleaseContext() {
  FB_LOG_DEBUG("Releasing OpenGL context");
#if defined(GPUPIXEL_ANDROID)
  if (egl_display_ != EGL_NO_DISPLAY) {
    eglMakeCurrent(egl_display_, EGL_NO_SURFACE, EGL_NO_SURFACE,
                   EGL_NO_CONTEXT);

    if (egl_surface_ != EGL_NO_SURFACE) {
      FB_LOG_TRACE("Destroying EGL surface");
      eglDestroySurface(egl_display_, egl_surface_);
      egl_surface_ = EGL_NO_SURFACE;
    }

    if (egl_context_ != EGL_NO_CONTEXT) {
      FB_LOG_TRACE("Destroying EGL context");
      eglDestroyContext(egl_display_, egl_context_);
      egl_context_ = EGL_NO_CONTEXT;
    }

    FB_LOG_TRACE("Terminating EGL display");
    eglTerminate(egl_display_);
    egl_display_ = EGL_NO_DISPLAY;
  }
#elif defined(GPUPIXEL_WIN) || defined(GPUPIXEL_LINUX)
  if (gl_context_) {
    FB_LOG_TRACE("Destroying GLFW window");
    glfwDestroyWindow(gl_context_);
  }
  FB_LOG_TRACE("Terminating GLFW");
  glfwTerminate();
#elif defined(GPUPIXEL_WASM)
  FB_LOG_TRACE("Destroying WebGL context");
  emscripten_webgl_destroy_context(wasm_context_);
#endif
  FB_LOG_INFO("OpenGL context released successfully");
}

void GPUPixelContext::SyncRunWithContext(std::function<void(void)> task) {
#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_MAC)
  if (!Util::IsAppleAppActive()) {
    return;
  }
#endif

#if defined(GPUPIXEL_WASM)
  FB_LOG_TRACE("Running task synchronously (WebGL)");
  UseAsCurrent();
  task();
#else
  FB_LOG_TRACE("Running task on task queue");
  task_queue_->runTask([=]() {
    UseAsCurrent();
    task();
  });
#endif
}
}  // namespace gpupixel
