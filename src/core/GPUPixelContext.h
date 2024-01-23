/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include <mutex>
#include "FramebufferCache.h"
#include "GPUPixelDef.h"
#include "DispatchQueue.h"

#include "Filter.h"
#include "GLProgram.h"

#if defined(GPUPIXEL_IOS)
  #import <OpenGLES/EAGL.h>
  #import <OpenGLES/ES3/gl.h>
#endif

#if defined(GPUPIXEL_MAC)
  #import <AppKit/NSOpenGL.h>
  #import <CoreMedia/CoreMedia.h>
  #import <OpenGL/gl.h>
  #import <QuartzCore/QuartzCore.h>
#elif defined(GPUPIXEL_LINUX)
#define GL_GLEXT_PROTOTYPES
  #define GLEW_STATIC
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

#if defined(GPUPIXEL_WIN)
#define GLEW_STATIC
  #include <GL/glew.h>
  #include <GLFW/glfw3.h>
#endif
 
NS_GPUPIXEL_BEGIN

class GPUPixelContext {
 public:
  static GPUPixelContext* getInstance();
  static void destroy();

  FramebufferCache* getFramebufferCache() const;
  void setActiveShaderProgram(GLProgram* shaderProgram);
  void purge();


  void runSync(std::function<void(void)> func);
  void runAsync(std::function<void(void)> func);
  void useAsCurrent(void);
#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_MAC)
  void presentBufferForDisplay();
#endif

#if defined(GPUPIXEL_IOS)
  EAGLContext* getEglContext() const { return _eglContext; };
#endif
  
#if defined(GPUPIXEL_MAC)
  NSOpenGLContext* getOpenGLContext() const { return imageProcessingContext; };
#endif
  
#if defined(GPUPIXEL_WIN)
  GLFWwindow* getShareContext() const { return _wglShareContext; };
#endif
  
#if defined(GPUPIXEL_ANDROID)

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

#if defined(GPUPIXEL_ANDROID) || (defined GPUPIXEL_WIN)
  void createContext();
  void releaseContext();
#endif
 private:
  static GPUPixelContext* _instance;
  static std::mutex _mutex;
  FramebufferCache* _framebufferCache;
  GLProgram* _curShaderProgram;

  
#if defined(GPUPIXEL_ANDROID)
  bool context_inited = false;
#endif

#if defined(GPUPIXEL_IOS)
  EAGLContext* _eglContext;
#endif
  
#if defined(GPUPIXEL_MAC)
  NSOpenGLContext* imageProcessingContext;
  NSOpenGLPixelFormat* _pixelFormat;
#endif
  
#if defined(GPUPIXEL_ANDROID)
  int m_surfacewidth;
  int m_surfaceheight;
  _gpu_context_t* m_gpu_context;
#endif
  
#if defined(GPUPIXEL_WIN)
  GLFWwindow* _wglShareContext = nullptr;
#endif
 std::shared_ptr<LocalDispatchQueue> task_queue_;
};

NS_GPUPIXEL_END
