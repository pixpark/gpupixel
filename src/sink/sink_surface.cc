/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

/**
 * @file sink_surface.cc
 * @brief Implementation file for the SinkSurface class on Android platform
 * 
 * Implements the functionality to directly render GPU processed images to Android Surface.
 * Main process:
 * 1. Get ANativeWindow from Android Surface
 * 2. Create EGL Window Surface
 * 3. Render directly in C layer EGL context
 * 4. Swap buffers to display to screen
 * 
 * Advantages:
 * - Avoids copying data from C layer to Java layer
 * - Avoids rendering again in Java layer
 * - Unified EGL context, better performance
 */

#if defined(__ANDROID__) || defined(GPUPIXEL_ANDROID)

#include "gpupixel/sink/sink_surface.h"
#include "core/gpupixel_context.h"
#include "core/gpupixel_gl_include.h"
#include "core/gpupixel_program.h"
#include "gpupixel/filter/filter.h"
#include "utils/logging.h"
#include "utils/util.h"
#include <android/native_window.h>

namespace gpupixel {

/**
 * @brief Create SinkSurface instance
 * @return Smart pointer to SinkSurface
 * 
 * Initialize shader program in GPUPixel's EGL context
 */
std::shared_ptr<SinkSurface> SinkSurface::Create() {
  auto ret = std::shared_ptr<SinkSurface>(new SinkSurface());
  // init int GPUPixel's的EGL Context
  gpupixel::GPUPixelContext::GetInstance()->SyncRunWithContext(
      [&] { ret->Init(); });
  return ret;
}

/**
 * @brief Constructor
 * Initialize all member variables to default values
 */
SinkSurface::SinkSurface()
    : view_width_(0),
      view_height_(0),
      fill_mode_(FillMode::PreserveAspectRatio),  // Default to preserve aspect ratio
      display_program_(0),
      position_attribute_location_(0),
      tex_coord_attribute_location_(0),
      color_map_uniform_location_(0),
      native_window_(nullptr),
      egl_surface_(EGL_NO_SURFACE),
      surface_created_(false) {
  // 默认背景色为黑色透明
  background_color_.r = 0.0;
  background_color_.g = 0.0;
  background_color_.b = 0.0;
  background_color_.a = 0.0;
}

SinkSurface::~SinkSurface() {
  ReleaseNativeWindow();
  if (display_program_) {
    delete display_program_;
    display_program_ = 0;
  }
}

/**
 * @brief Initialize rendering program
 * Create and compile default vertex and fragment shaders, get attribute locations
 */
void SinkSurface::Init() {
  // Create default display shader program
  display_program_ = GPUPixelGLProgram::CreateWithShaderString(
      kDefaultVertexShader, kDefaultFragmentShader);
  
  // Get the location of vertex position attribute in shader
  position_attribute_location_ =
      display_program_->GetAttribLocation("position");
  
  // Get the location of texture coordinate attribute in shader
  tex_coord_attribute_location_ =
      display_program_->GetAttribLocation("inputTextureCoordinate");
  
  // Get the location of texture sampler uniform in shader
  color_map_uniform_location_ =
      display_program_->GetUniformLocation("inputImageTexture");
  
  // Activate shader program
  GPUPixelContext::GetInstance()->SetActiveGlProgram(display_program_);
  
  // Enable vertex attribute arrays
  GL_CALL(glEnableVertexAttribArray(position_attribute_location_));
  GL_CALL(glEnableVertexAttribArray(tex_coord_attribute_location_));
}

void SinkSurface::SetInputFramebuffer(
    std::shared_ptr<GPUPixelFramebuffer> framebuffer,
    RotationMode rotation_mode /* = NoRotation*/,
    int tex_idx /* = 0*/) {
  std::shared_ptr<GPUPixelFramebuffer> last_input_framebuffer;
  RotationMode last_input_rotation = NoRotation;
  if (input_framebuffers_.find(0) != input_framebuffers_.end()) {
    last_input_framebuffer = input_framebuffers_[0].frame_buffer;
    last_input_rotation = input_framebuffers_[0].rotation_mode;
  }

  Sink::SetInputFramebuffer(framebuffer, rotation_mode, tex_idx);

  if (last_input_framebuffer != framebuffer && framebuffer &&
      (!last_input_framebuffer ||
       !(last_input_framebuffer->GetWidth() == framebuffer->GetWidth() &&
         last_input_framebuffer->GetHeight() == framebuffer->GetHeight() &&
         last_input_rotation == rotation_mode))) {
    UpdateDisplayVertices();
  }
}

void SinkSurface::SetFillMode(FillMode fill_mode) {
  if (fill_mode_ != fill_mode) {
    fill_mode_ = fill_mode;
    UpdateDisplayVertices();
  }
}

void SinkSurface::SetMirror(bool mirror) {
  if (mirror_ != mirror) {
    mirror_ = mirror;
  }
}

/**
 * @brief Set Android native window
 * @param window ANativeWindow pointer (obtained from Android Surface)
 * @param width Window width (in pixels)
 * @param height Window height (in pixels)
 * 
 * If the window has not changed, return directly; otherwise, first release the old window, then create a new EGL Surface
 */
void SinkSurface::SetNativeWindow(ANativeWindow* window, int width, int height) {
  // If the window and dimensions haven't changed, return directly
  if (native_window_ == window && view_width_ == width && view_height_ == height) {
    return;
  }
  
  // Release old window resources
  ReleaseNativeWindow();
  
  // Set new window parameters
  native_window_ = window;
  view_width_ = width;
  view_height_ = height;
  
  // If window is valid, create EGL Surface and update display vertices
  if (native_window_ != nullptr) {
    CreateEGLSurface();
    UpdateDisplayVertices();
  }
}

void SinkSurface::ReleaseNativeWindow() {
  DestroyEGLSurface();
  if (native_window_ != nullptr) {
    ANativeWindow_release(native_window_);
    native_window_ = nullptr;
  }
  view_width_ = 0;
  view_height_ = 0;
}

/**
 * @brief Create EGL Window Surface
 * @return Whether creation was successful
 * 
 * Create EGL Window Surface from ANativeWindow, used for rendering to Android Surface
 */
bool SinkSurface::CreateEGLSurface() {
  if (native_window_ == nullptr) {
    LOG_ERROR("SinkSurface: native_window_ is null");
    return false;
  }
  
  // If surface has already been created, return directly
  if (surface_created_) {
    LOG_WARN("SinkSurface: EGL surface already created");
    return true;
  }
  
  // Get GPUPixel's EGL context information
  GPUPixelContext* context = GPUPixelContext::GetInstance();
  EGLDisplay egl_display = context->GetEglDisplay();
  EGLConfig egl_config = context->GetEglConfig();
  
  // Create EGL Window Surface from native window
  // No additional surface attributes needed
  const EGLint surface_attribs[] = {
      EGL_NONE
  };
  
  egl_surface_ = eglCreateWindowSurface(egl_display, egl_config,
                                       native_window_, surface_attribs);
  if (egl_surface_ == EGL_NO_SURFACE) {
    // Creation failed, log error code
    EGLint error = eglGetError();
    LOG_ERROR("SinkSurface: Failed to create EGL window surface, error: {}", error);
    return false;
  }
  
  surface_created_ = true;
  LOG_INFO("SinkSurface: EGL window surface created successfully");
  return true;
}

void SinkSurface::DestroyEGLSurface() {
  if (!surface_created_ || egl_surface_ == EGL_NO_SURFACE) {
    return;
  }
  
  GPUPixelContext* context = GPUPixelContext::GetInstance();
  EGLDisplay egl_display = context->GetEglDisplay();
  EGLContext egl_context = context->GetEglContext();
  
  // Make sure we're not using this surface
  if (eglGetCurrentSurface(EGL_DRAW) == egl_surface_) {
    EGLSurface default_surface = context->GetEglSurface();
    eglMakeCurrent(egl_display, default_surface, default_surface, egl_context);
  }
  
  eglDestroySurface(egl_display, egl_surface_);
  egl_surface_ = EGL_NO_SURFACE;
  surface_created_ = false;
  
  LOG_INFO("SinkSurface: EGL window surface destroyed");
}

/**
 * @brief Perform rendering
 * 
 * Render the content of the input framebuffer to the EGL Window Surface, then swap buffers to display to screen.
 * This is the core function of the entire rendering process, called after GPUPixel's filter chain processing is complete.
 */
void SinkSurface::Render() {
  // Check if Surface has been created
  if (!surface_created_ || egl_surface_ == EGL_NO_SURFACE) {
    LOG_WARN("SinkSurface: Cannot render, surface not created");
    return;
  }
  
  // Check if view dimensions are valid
  if (view_width_ == 0 || view_height_ == 0) {
    LOG_WARN("SinkSurface: view_width_ or view_height_ is 0");
    return;
  }
  
  // Check if there is an input framebuffer
  if (input_framebuffers_.find(0) == input_framebuffers_.end() ||
      input_framebuffers_[0].frame_buffer == nullptr) {
    LOG_WARN("SinkSurface: No input framebuffer");
    return;
  }
  
  // Get EGL context information
  GPUPixelContext* context = GPUPixelContext::GetInstance();
  EGLDisplay egl_display = context->GetEglDisplay();
  EGLContext egl_context = context->GetEglContext();
  
  // Switch to Window Surface for rendering
  // This is the key context switch, from off-screen rendering to window rendering
  if (!eglMakeCurrent(egl_display, egl_surface_, egl_surface_, egl_context)) {
    EGLint error = eglGetError();
    LOG_ERROR("SinkSurface: Failed to make EGL context current, error: {}", error);
    return;
  }
  
  // Set viewport
  GL_CALL(glViewport(0, 0, view_width_, view_height_));
  
  // Clear color and depth buffers
  GL_CALL(glClearColor(background_color_.r, background_color_.g,
                       background_color_.b, background_color_.a));
  GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
  
  // Activate the display shader program
  context->SetActiveGlProgram(display_program_);
  
  // Bind input texture
  GL_CALL(glActiveTexture(GL_TEXTURE0));
  GL_CALL(glBindTexture(GL_TEXTURE_2D,
                        input_framebuffers_[0].frame_buffer->GetTexture()));
  GL_CALL(glUniform1i(color_map_uniform_location_, 0));
  
  // Set vertex attributes
  GL_CALL(glVertexAttribPointer(position_attribute_location_, 2, GL_FLOAT, 0, 0,
                                display_vertices_));
  GL_CALL(glVertexAttribPointer(
      tex_coord_attribute_location_, 2, GL_FLOAT, 0, 0,
      GetTextureCoordinate(input_framebuffers_[0].rotation_mode)));

  // Draw two triangles forming a rectangle (using TRIANGLE_STRIP mode with 4 vertices)
  GL_CALL(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));
  
  // Swap buffers to display the rendered result to screen
  eglSwapBuffers(egl_display, egl_surface_);

  // After rendering, restore to the default off-screen Surface to ensure subsequent off-screen rendering executes properly
  EGLSurface default_surface = context->GetEglSurface();
  if (default_surface != EGL_NO_SURFACE) {
    if (!eglMakeCurrent(egl_display, default_surface, default_surface,
                        egl_context)) {
      EGLint error = eglGetError();
      LOG_ERROR("SinkSurface: Failed to restore default EGL surface, error: {}",
                error);
    }
  }
}

/**
 * @brief Update display vertex coordinates
 * 
 * Calculate display vertex coordinates based on input framebuffer dimensions, rotation mode and fill mode.
 * Vertex coordinates are normalized (-1 to 1 range), used for OpenGL rendering.
 */
void SinkSurface::UpdateDisplayVertices() {
  // Check if there is an input framebuffer
  if (input_framebuffers_.find(0) == input_framebuffers_.end() ||
      input_framebuffers_[0].frame_buffer == nullptr) {
    return;
  }

  std::shared_ptr<GPUPixelFramebuffer> input_framebuffer =
      input_framebuffers_[0].frame_buffer;
  RotationMode input_rotation = input_framebuffers_[0].rotation_mode;

  // Calculate rotated framebuffer dimensions
  int rotated_framebuffer_width = input_framebuffer->GetWidth();
  int rotated_framebuffer_height = input_framebuffer->GetHeight();
  if (rotationSwapsSize(input_rotation)) {
    // 90 or 270 degree rotation swaps width and height
    rotated_framebuffer_width = input_framebuffer->GetHeight();
    rotated_framebuffer_height = input_framebuffer->GetWidth();
  }

  // Calculate aspect ratios
  float framebuffer_aspect_ratio =
      rotated_framebuffer_height / (float)rotated_framebuffer_width;
  float view_aspect_ratio = view_height_ / (float)view_width_;

  // Calculate inscribed dimensions when preserving aspect ratio
  float inset_framebuffer_width = 0.0;
  float inset_framebuffer_height = 0.0;
  if (framebuffer_aspect_ratio > view_aspect_ratio) {
    // Framebuffer is taller, fit to height
    inset_framebuffer_width = view_height_ / (float)rotated_framebuffer_height *
                              rotated_framebuffer_width;
    inset_framebuffer_height = view_height_;
  } else {
    // Framebuffer is wider, fit to width
    inset_framebuffer_width = view_width_;
    inset_framebuffer_height = view_width_ / (float)rotated_framebuffer_width *
                               rotated_framebuffer_height;
  }

  // Calculate scaling based on fill mode
  float scaled_width = 1.0;
  float scaled_height = 1.0;
  if (fill_mode_ == FillMode::PreserveAspectRatio) {
    // Preserve aspect ratio, may have black borders
    scaled_width = inset_framebuffer_width / view_width_;
    scaled_height = inset_framebuffer_height / view_height_;
  } else if (fill_mode_ == FillMode::PreserveAspectRatioAndFill) {
    // Preserve aspect ratio and scale to fill, may crop
    scaled_width = view_width_ / inset_framebuffer_height;
    scaled_height = view_height_ / inset_framebuffer_width;
  }
  // In Stretch mode, scaled_width and scaled_height remain 1.0

  // Set normalized coordinates for 4 vertices (bottom-left, bottom-right, top-left, top-right)
  display_vertices_[0] = -scaled_width;   // bottom-left x
  display_vertices_[1] = -scaled_height;  // bottom-left y
  display_vertices_[2] = scaled_width;    // bottom-right x
  display_vertices_[3] = -scaled_height;  // bottom-right y
  display_vertices_[4] = -scaled_width;   // top-left x
  display_vertices_[5] = scaled_height;   // top-left y
  display_vertices_[6] = scaled_width;    // top-right x
  display_vertices_[7] = scaled_height;  // top-right y
}

const float* SinkSurface::GetTextureCoordinate(RotationMode rotation_mode) {
  static const float no_rotation_texture_coordinates[] = {
      0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
  };

  static const float rotate_right_texture_coordinates[] = {
      1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
  };

  static const float rotate_left_texture_coordinates[] = {
      0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
  };

  static const float vertical_flip_texture_coordinates[] = {
      0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
  };

  static const float horizontal_flip_texture_coordinates[] = {
      1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
  };

  static const float rotate_right_vertical_flip_texture_coordinates[] = {
      1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
  };

  static const float rotate_right_horizontal_flip_texture_coordinates[] = {
      0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f,
  };

  static const float rotate_180_texture_coordinates[] = {
      1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
  };

  switch (rotation_mode) {
    case NoRotation: {
      if (mirror_) {
        return horizontal_flip_texture_coordinates;
      } else {
        return no_rotation_texture_coordinates;
      }
    }
    case RotateLeft:
      return rotate_left_texture_coordinates;
    case RotateRight:
      return rotate_right_texture_coordinates;
    case FlipVertical:
      return vertical_flip_texture_coordinates;
    case FlipHorizontal:
      return horizontal_flip_texture_coordinates;
    case RotateRightFlipVertical:
      return rotate_right_vertical_flip_texture_coordinates;
    case RotateRightFlipHorizontal:
      return rotate_right_horizontal_flip_texture_coordinates;
    case Rotate180:
      return rotate_180_texture_coordinates;
  }
}

}  // namespace gpupixel

#endif  // GPUPIXEL_ANDROID

