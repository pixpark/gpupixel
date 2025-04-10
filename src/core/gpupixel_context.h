/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include <mutex>
#include "core/gpupixel_framebuffer_factory.h"
#include "gpupixel/gpupixel_define.h"

#include "core/gpupixel_program.h"
#include "gpupixel/filter/filter.h"

class DispatchQueue;

namespace gpupixel {

class GPUPIXEL_API GPUPixelContext {
 public:
  static GPUPixelContext* GetInstance();
  static void Destroy();

  FramebufferFactory* GetFramebufferFactory() const;
  void SetActiveGlProgram(GPUPixelGLProgram* shaderProgram);
  void Clean();

  void SyncRunWithContext(std::function<void(void)> func);
  void UseAsCurrent(void);
  void PresentBufferForDisplay();

#if defined(GPUPIXEL_IOS)
  EAGLContext* GetEglContext() const { return egl_context_; };
#elif defined(GPUPIXEL_MAC)
  NSOpenGLContext* GetOpenGLContext() const {
    return image_processing_context_;
  };
#elif defined(GPUPIXEL_ANDROID)
  EGLContext GetEglContext() const { return egl_context_; };
  EGLDisplay GetEglDisplay() const { return egl_display_; };
  EGLSurface GetEglSurface() const { return egl_surface_; };
#elif defined(GPUPIXEL_WIN) || defined(GPUPIXEL_LINUX)
  GLFWwindow* GetGLContext() const { return gl_context_; };
#endif

 private:
  GPUPixelContext();
  ~GPUPixelContext();

  void Init();

  void CreateContext();
  void ReleaseContext();

 private:
  static GPUPixelContext* instance_;
  static std::mutex mutex_;
  FramebufferFactory* framebuffer_factory_;
  GPUPixelGLProgram* current_shader_program_;
  std::shared_ptr<DispatchQueue> task_queue_;

#if defined(GPUPIXEL_IOS)
  EAGLContext* egl_context_;
#elif defined(GPUPIXEL_MAC)
  NSOpenGLContext* image_processing_context_;
  NSOpenGLPixelFormat* pixel_format_;
#elif defined(GPUPIXEL_ANDROID)
  EGLContext egl_context_;
  EGLDisplay egl_display_;
  EGLSurface egl_surface_;
  EGLConfig egl_config_;
#elif defined(GPUPIXEL_WIN) || defined(GPUPIXEL_LINUX)
  GLFWwindow* gl_context_ = nullptr;
#endif
};

}  // namespace gpupixel
