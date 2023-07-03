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
#if !defined(GPUPIXEL_WIN)
#include <pthread.h>
#endif
#include "Filter.h"
#include "GLProgram.h"

#if defined(GPUPIXEL_IOS)
#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES3/gl.h>

#elif defined(GPUPIXEL_MAC)
#import <AppKit/NSOpenGL.h>
#import <CoreMedia/CoreMedia.h>
#import <OpenGL/gl.h>
#import <QuartzCore/QuartzCore.h>

#elif defined(GPUPIXEL_ANDROID)
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

#elif defined(GPUPIXEL_WIN)
#include <glew/glew.h>
#include <glfw/glfw3.h>
#else
#endif
 
NS_GPUPIXEL_BEGIN

class GPUPixelContext {
 public:
  static GPUPixelContext* getInstance();
  static void destroy();

  FramebufferCache* getFramebufferCache() const;
  void setActiveShaderProgram(GLProgram* shaderProgram);
  void purge();

#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_MAC)
  void runSync(std::function<void(void)> func);
  void runAsync(std::function<void(void)> func);
  void useAsCurrent(void);
  dispatch_queue_t getContextQueue() const { return _contextQueue; };
  void presentBufferForDisplay();
#elif defined(GPUPIXEL_ANDROID)
  void runSync(std::function<void(void)> func);
  void runAsync(std::function<void(void)> func);
  void useAsCurrent(void);
#elif defined(GPUPIXEL_WIN)
  void runSync(std::function<void(void)> func);
  void runAsync(std::function<void(void)> func);
  void useAsCurrent();
#endif

#if defined(GPUPIXEL_IOS)
  EAGLContext* getEglContext() const { return _eglContext; };
#elif defined(GPUPIXEL_MAC)
  NSOpenGLContext* getOpenGLContext() const { return imageProcessingContext; };
#elif defined(GPUPIXEL_WIN)
  GLFWwindow* getShareContext() const { return _wglShareContext; };
#elif defined(GPUPIXEL_ANDROID)

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

#if (defined __ANDROID__) || (defined GPUPIXEL_WIN)
  void createContext();
  void releaseContext();
#endif
 private:
  static GPUPixelContext* _instance;
  static std::mutex _mutex;
  FramebufferCache* _framebufferCache;
  GLProgram* _curShaderProgram;

#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_MAC)
  dispatch_queue_t _contextQueue;
#elif (defined __ANDROID__)
 
  bool context_inited = false;
#elif defined(GPUPIXEL_WIN)

#endif

#if defined(GPUPIXEL_IOS)
  EAGLContext* _eglContext;
#elif defined(GPUPIXEL_MAC)
  NSOpenGLContext* imageProcessingContext;
  NSOpenGLPixelFormat* _pixelFormat;
#elif defined(GPUPIXEL_ANDROID)
  int m_surfacewidth;
  int m_surfaceheight;
  _gpu_context_t* m_gpu_context;
#elif defined(GPUPIXEL_WIN)
  GLFWwindow* _wglShareContext = nullptr;
#else
#endif
};

NS_GPUPIXEL_END
