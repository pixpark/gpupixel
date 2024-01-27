/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include <mutex>
#include "framebuffer_cache.h"
#include "gpupixel_macros.h"
#include "dispatch_queue.h"

#include "filter.h"
#include "gl_program.h"

#if defined(GPUPIXEL_IOS)
  #import <OpenGLES/EAGL.h>
  #import <OpenGLES/ES3/gl.h>
#endif

#if defined(GPUPIXEL_MAC)
//  #import <CoreMedia/CoreMedia.h>
  #import <OpenGL/gl3.h>
//  #import <QuartzCore/QuartzCore.h>
#elif defined(GPUPIXEL_WIN) || defined(GPUPIXEL_LINUX)
  #define GL_GLEXT_PROTOTYPES
  #define GLEW_STATIC
  #define GLFW_INCLUDE_GLCOREARB
  #include <GLFW/glfw3.h>
#endif

#if defined(GPUPIXEL_ANDROID)
  #include <EGL/egl.h>
  #include <GLES/gl.h>
  #include <GLES/glext.h>
  #include <GLES3/gl3.h>
  #include <GLES3/gl3ext.h>
  #include <android/log.h>
  #include <jni.h>

  typedef struct _gpu_context_t {
    EGLDisplay egldisplay;
    EGLSurface eglsurface;
    EGLContext eglcontext;
  } _gpu_context_t;
#endif
 
 
NS_GPUPIXEL_BEGIN

class GPUPixelContext {
 public:
  static GPUPixelContext* getInstance();
  static void destroy();

  FramebufferCache* getFramebufferCache() const;
  //todo(zhaoyou)
  void setActiveShaderProgram(GLProgram* shaderProgram);
  void purge();

  void runSync(std::function<void(void)> func);
  void runAsync(std::function<void(void)> func);
  void useAsCurrent(void);
  void presentBufferForDisplay();
 
#if defined(GPUPIXEL_IOS)
  EAGLContext* getEglContext() const { return _eglContext; };
#elif defined(GPUPIXEL_MAC)
  NSOpenGLContext* getOpenGLContext() const { return imageProcessingContext; };
#elif defined(GPUPIXEL_WIN) || defined(GPUPIXEL_LINUX)
  GLFWwindow* getShareContext() const { return _wglShareContext; };
#endif
 

  // used for capturing a processed frame data
  bool isCapturingFrame;
  std::shared_ptr<Filter> captureUpToFilter;
  unsigned char* capturedFrameData;
  int captureWidth;
  int captureHeight;

 private:
  GPUPixelContext();
  ~GPUPixelContext();

  void init();

  void createContext();
  void releaseContext();
 private:
  static GPUPixelContext* _instance;
  static std::mutex _mutex;
  FramebufferCache* _framebufferCache;
  GLProgram* _curShaderProgram;
  std::shared_ptr<LocalDispatchQueue> task_queue_;
  
#if defined(GPUPIXEL_ANDROID)
  bool context_inited = false;
  int m_surfacewidth;
  int m_surfaceheight;
  _gpu_context_t* m_gpu_context;
#elif defined(GPUPIXEL_IOS)
  EAGLContext* _eglContext;
#elif defined(GPUPIXEL_MAC)
  NSOpenGLContext* imageProcessingContext;
  NSOpenGLPixelFormat* _pixelFormat;
#elif defined(GPUPIXEL_WIN) || defined(GPUPIXEL_LINUX)
  GLFWwindow* _wglShareContext = nullptr;
#endif

};

NS_GPUPIXEL_END
