/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include <mutex>
#include "gpupixel_define.h"
#include "gpupixel_framebuffer_factory.h"

#include "filter.h"
#include "gpupixel_program.h"

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

  // used for capturing a processed frame data
  bool is_capturing_frame_;
  std::shared_ptr<Filter> capture_frame_filter_;
  unsigned char* capture_frame_data_;
  int capture_width_;
  int capture_height_;

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
