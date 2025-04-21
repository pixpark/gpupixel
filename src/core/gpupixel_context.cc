/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "core/gpupixel_context.h"
#include "utils/dispatch_queue.h"
#include "utils/util.h"
#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_MAC)

// Helper class to manage iOS application state
@interface iOSHelper : NSObject {
  bool isActive;
}

- (bool)isAppActive;
@end

@implementation iOSHelper
- (id)init {
  if (self = [super init]) {
#if defined(GPUPIXEL_IOS)
    // Register for application state change notifications
    [[NSNotificationCenter defaultCenter]
        addObserver:self
           selector:@selector(willResignActive)
               name:UIApplicationWillResignActiveNotification
             object:nil];
    [[NSNotificationCenter defaultCenter]
        addObserver:self
           selector:@selector(didBecomeActive)
               name:UIApplicationDidBecomeActiveNotification
             object:nil];
#endif
    isActive = true;
  }
  return self;
}

// Handle application state changes
- (void)willResignActive {
  @synchronized(self) {
    isActive = false;
  }
}

- (void)didBecomeActive {
  @synchronized(self) {
    isActive = true;
  }
}

- (bool)isAppActive {
  @synchronized(self) {
    return isActive;
  }
}

- (void)dealloc {
#if defined(GPUPIXEL_IOS)
  // Remove notification observers
  [[NSNotificationCenter defaultCenter]
      removeObserver:self
                name:UIApplicationWillResignActiveNotification
              object:nil];
  [[NSNotificationCenter defaultCenter]
      removeObserver:self
                name:UIApplicationDidBecomeActiveNotification
              object:nil];
#endif
}
@end

#endif

namespace gpupixel {

#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_MAC)
iOSHelper* ios_helper_;
#elif defined(GPUPIXEL_WIN) || defined(GPUPIXEL_LINUX)
// Default view dimensions for Windows/Linux platforms
const unsigned int VIEW_WIDTH = 1280;
const unsigned int VIEW_HEIGHT = 720;

#endif

GPUPixelContext* GPUPixelContext::instance_ = 0;
std::mutex GPUPixelContext::mutex_;

GPUPixelContext::GPUPixelContext() : current_shader_program_(0) {
  task_queue_ = std::make_shared<DispatchQueue>();
  framebuffer_factory_ = new FramebufferFactory();
  Init();
}

GPUPixelContext::~GPUPixelContext() {
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
#if defined(GPUPIXEL_IOS)
    ios_helper_ = nil;
#endif
  }
}

void GPUPixelContext::Init() {
  SyncRunWithContext([=] {
    Util::Log("INFO", "start init GPUPixelContext");
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
  framebuffer_factory_->Clean();
}

void GPUPixelContext::CreateContext() {
#if defined(GPUPIXEL_IOS)
  egl_context_ = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
  [EAGLContext setCurrentContext:egl_context_];
  ios_helper_ = [[iOSHelper alloc] init];
#elif defined(GPUPIXEL_MAC)
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
  image_processing_context_ =
      [[NSOpenGLContext alloc] initWithFormat:pixel_format_ shareContext:nil];

  GLint interval = 0;
  [image_processing_context_ makeCurrentContext];
  [image_processing_context_ setValues:&interval
                          forParameter:NSOpenGLContextParameterSwapInterval];
#elif defined(GPUPIXEL_ANDROID)
  // Initialize EGL
  egl_display_ = eglGetDisplay(EGL_DEFAULT_DISPLAY);
  if (egl_display_ == EGL_NO_DISPLAY) {
    Util::Log("ERROR", "Failed to get EGL display");
    return;
  }

  EGLint major, minor;
  if (!eglInitialize(egl_display_, &major, &minor)) {
    Util::Log("ERROR", "Failed to initialize EGL");
    return;
  }

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
    Util::Log("ERROR", "Failed to choose EGL config");
    return;
  }

  // Create EGL context
  const EGLint contextAttribs[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};

  egl_context_ = eglCreateContext(egl_display_, egl_config_, EGL_NO_CONTEXT,
                                  contextAttribs);
  if (egl_context_ == EGL_NO_CONTEXT) {
    Util::Log("ERROR", "Failed to create EGL context");
    return;
  }

  // Create offscreen rendering surface
  const EGLint pbufferAttribs[] = {EGL_WIDTH, 1, EGL_HEIGHT, 1, EGL_NONE};

  egl_surface_ =
      eglCreatePbufferSurface(egl_display_, egl_config_, pbufferAttribs);
  if (egl_surface_ == EGL_NO_SURFACE) {
    Util::Log("ERROR", "Failed to create EGL surface");
    return;
  }

  // Set current context
  if (!eglMakeCurrent(egl_display_, egl_surface_, egl_surface_, egl_context_)) {
    Util::Log("ERROR", "Failed to make EGL context current");
    return;
  }
#elif defined(GPUPIXEL_WIN) || defined(GPUPIXEL_LINUX)
  int ret = glfwInit();

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

  if (ret) {
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
  } else {
    // todo log error
    return;
  }
  gl_context_ = glfwCreateWindow(VIEW_WIDTH, VIEW_HEIGHT,
                                 "gpupixel opengl context", NULL, NULL);
  if (!gl_context_) {
    // todo log error
    glfwTerminate();
    return;
  }
  glfwMakeContextCurrent(gl_context_);

  gladLoadGL();
#endif
}

void GPUPixelContext::UseAsCurrent() {
#if defined(GPUPIXEL_IOS)
  if ([EAGLContext currentContext] != egl_context_) {
    [EAGLContext setCurrentContext:egl_context_];
  }
#elif defined(GPUPIXEL_MAC)
  if ([NSOpenGLContext currentContext] != image_processing_context_) {
    [image_processing_context_ makeCurrentContext];
  }
#elif defined(GPUPIXEL_ANDROID)
  if (eglGetCurrentContext() != egl_context_) {
    eglMakeCurrent(egl_display_, egl_surface_, egl_surface_, egl_context_);
  }
#elif defined(GPUPIXEL_WIN) || defined(GPUPIXEL_LINUX)
  if (glfwGetCurrentContext() != gl_context_) {
    glfwMakeContextCurrent(gl_context_);
  }
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
#if defined(GPUPIXEL_ANDROID)
  if (egl_display_ != EGL_NO_DISPLAY) {
    eglMakeCurrent(egl_display_, EGL_NO_SURFACE, EGL_NO_SURFACE,
                   EGL_NO_CONTEXT);

    if (egl_surface_ != EGL_NO_SURFACE) {
      eglDestroySurface(egl_display_, egl_surface_);
      egl_surface_ = EGL_NO_SURFACE;
    }

    if (egl_context_ != EGL_NO_CONTEXT) {
      eglDestroyContext(egl_display_, egl_context_);
      egl_context_ = EGL_NO_CONTEXT;
    }

    eglTerminate(egl_display_);
    egl_display_ = EGL_NO_DISPLAY;
  }
#elif defined(GPUPIXEL_WIN) || defined(GPUPIXEL_LINUX)
  if (gl_context_) {
    glfwDestroyWindow(gl_context_);
  }
  glfwTerminate();
#endif
}

void GPUPixelContext::SyncRunWithContext(std::function<void(void)> task) {
  task_queue_->runTask([=]() {
    UseAsCurrent();
    task();
  });
}

}  // namespace gpupixel
