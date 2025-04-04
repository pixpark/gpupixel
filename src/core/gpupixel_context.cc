/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "gpupixel_context.h"
#include "util.h"

#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_MAC)

typedef void (^TaskBlock)(void);

@interface iOSHelper : NSObject {
  bool isActive;
}

- (bool)isAppActive;
- (void)run:(TaskBlock)task;
@end

@implementation iOSHelper
- (id)init {
  if (self = [super init]) {
#if defined(GPUPIXEL_IOS)
    // register notification
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

- (void)run:(TaskBlock)task {
  [self performSelectorOnMainThread:@selector(doRunTask:)
                         withObject:task
                      waitUntilDone:NO];
}
- (void)doRunTask:(TaskBlock)task {
  task();
}

- (void)dealloc {
#if defined(GPUPIXEL_IOS)
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
const unsigned int VIEW_WIDTH = 1280;
const unsigned int VIEW_HEIGHT = 720;

#endif

GPUPixelContext* GPUPixelContext::instance_ = 0;
std::mutex GPUPixelContext::mutex_;

GPUPixelContext::GPUPixelContext()
    : current_shader_program_(0),
      is_capturing_frame_(false),
      capture_frame_filter_(0),
      capture_frame_data_(0) {
  framebuffer_factory_ = new FramebufferFactory();
  Init();
}

GPUPixelContext::~GPUPixelContext() {
  ReleaseContext();
  delete framebuffer_factory_;
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
  RunSync([=] {
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
#endif
}

void GPUPixelContext::ReleaseContext() {
#if defined(GPUPIXEL_WIN) || defined(GPUPIXEL_LINUX)
  if (gl_context_) {
    glfwDestroyWindow(gl_context_);
  }
  glfwTerminate();
#endif
}

void GPUPixelContext::RunSync(std::function<void(void)> func) {
#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_MAC)
  if ([NSThread isMainThread]) {
    UseAsCurrent();
    func();
  } else {
    dispatch_sync(dispatch_get_main_queue(), ^{
      UseAsCurrent();
      func();
    });
  }
#elif defined(GPUPIXEL_WIN) || defined(GPUPIXEL_LINUX)
  UseAsCurrent();
  func();
#else
  func();
#endif
}

}  // namespace gpupixel
