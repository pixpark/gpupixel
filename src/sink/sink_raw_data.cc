//
//  SourceRawOutput.cc
//  GPUPixel iOS
//
//  Created by PixPark on 2021/6/24.
//  Copyright © 2021 PixPark. All rights reserved.
//

#include "sink_raw_data.h"
#include <cstring>
#include "gpupixel_context.h"
#include "libyuv.h"

namespace gpupixel {

const std::string kRGBToI420VertexShaderString = R"(
    attribute vec4 position;
    attribute vec4 inputTextureCoordinate;
    varying vec2 textureCoordinate;

    void main() {
      gl_Position = position;
      textureCoordinate = inputTextureCoordinate.xy;
    })";

#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_ANDROID)
const std::string kRGBToI420FragmentShaderString = R"(
    varying mediump vec2 textureCoordinate;
    uniform sampler2D sTexture;
    void main() {
      gl_FragColor = texture2D(sTexture, textureCoordinate);
    })";
#elif defined(GPUPIXEL_MAC) || defined(GPUPIXEL_WIN) || defined(GPUPIXEL_LINUX)
const std::string kRGBToI420FragmentShaderString = R"(
    varying vec2 textureCoordinate;
    uniform sampler2D sTexture;
    void main() {
      gl_FragColor = texture2D(sTexture, textureCoordinate);
    })";
#endif

std::shared_ptr<SinkRawData> SinkRawData::Create() {
  std::shared_ptr<SinkRawData> ret;
  gpupixel::GPUPixelContext::GetInstance()->SyncRunWithContext(
      [&] { ret = std::shared_ptr<SinkRawData>(new SinkRawData()); });
  return ret;
}

SinkRawData::SinkRawData() {
  InitWithShaderString(kRGBToI420VertexShaderString,
                       kRGBToI420FragmentShaderString);
}

SinkRawData::~SinkRawData() {
  // Clean up RGBA frame buffer
  if (rgba_buffer_ != nullptr) {
    delete[] rgba_buffer_;
  }
  rgba_buffer_ = nullptr;

  // Clean up YUV frame buffer
  if (yuv_buffer_ != nullptr) {
    delete[] yuv_buffer_;
  }
  yuv_buffer_ = nullptr;
}

void SinkRawData::Render() {
  if (input_framebuffers_.empty()) {
    return;
  }

  int width = input_framebuffers_[0].frame_buffer->GetWidth();
  int height = input_framebuffers_[0].frame_buffer->GetHeight();
  if (width_ != width || height_ != height) {
    width_ = width;
    height_ = height;
    InitFramebuffer(width, height);
    InitOutputBuffer(width, height);
  }

  RenderToOutput();
}

bool SinkRawData::InitWithShaderString(
    const std::string& vertex_shader_source,
    const std::string& fragment_shader_source) {
  shader_program_ = GPUPixelGLProgram::CreateWithShaderString(
      vertex_shader_source, fragment_shader_source);
  GPUPixelContext::GetInstance()->SetActiveGlProgram(shader_program_);
  position_attribute_ = shader_program_->GetAttribLocation("position");
  tex_coord_attribute_ =
      shader_program_->GetAttribLocation("inputTextureCoordinate");

  return true;
}

int SinkRawData::RenderToOutput() {
  GPUPixelContext::GetInstance()->SetActiveGlProgram(shader_program_);
  framebuffer_->Activate();

  CHECK_GL(glViewport(0, 0, width_, height_));

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  GLfloat image_vertices[] = {
      -1.0, -1.0,  // Bottom left
      1.0,  -1.0,  // Bottom right
      -1.0, 1.0,   // Top left
      1.0,  1.0    // Top right
  };

  GLfloat texture_vertices[] = {
      0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
  };

  CHECK_GL(glEnableVertexAttribArray(position_attribute_));
  CHECK_GL(glVertexAttribPointer(position_attribute_, 2, GL_FLOAT, 0, 0,
                                 image_vertices));

  CHECK_GL(glEnableVertexAttribArray(tex_coord_attribute_));
  CHECK_GL(glVertexAttribPointer(tex_coord_attribute_, 2, GL_FLOAT, 0, 0,
                                 texture_vertices));

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D,
                input_framebuffers_[0].frame_buffer->GetTexture());

  CHECK_GL(shader_program_->SetUniformValue("sTexture", 0));
  // Draw frame buffer
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  // Read pixel data directly using glReadPixels
  CHECK_GL(glReadPixels(0, 0, width_, height_, GL_RGBA, GL_UNSIGNED_BYTE,
                        rgba_buffer_));

  framebuffer_->Deactivate();
  return 0;
}

const uint8_t* SinkRawData::GetRgbaBuffer() const {
  return rgba_buffer_;
}

const uint8_t* SinkRawData::GetI420Buffer() const {
  // Convert RGBA to I420 format
  libyuv::ARGBToI420(rgba_buffer_, width_ * 4, yuv_buffer_, width_,
                     yuv_buffer_ + width_ * height_, width_ / 2,
                     yuv_buffer_ + width_ * height_ * 5 / 4, width_ / 2, width_,
                     height_);

  return yuv_buffer_;
}

void SinkRawData::InitOutputBuffer(int width, int height) {
  uint32_t rgba_size = width * height * 4;
  uint32_t yuv_size = width * height * 3 / 2;

  // Allocate RGBA frame buffer
  if (rgba_buffer_ != nullptr) {
    delete[] rgba_buffer_;
  }
  rgba_buffer_ = new uint8_t[rgba_size];
  std::memset(rgba_buffer_, 0, rgba_size);

  // Allocate YUV frame buffer
  if (yuv_buffer_ != nullptr) {
    delete[] yuv_buffer_;
  }
  yuv_buffer_ = new uint8_t[yuv_size];
  std::memset(yuv_buffer_, 0, yuv_size);
}

void SinkRawData::InitFramebuffer(int width, int height) {
  if (!framebuffer_ || (framebuffer_->GetWidth() != width ||
                        framebuffer_->GetHeight() != height)) {
    framebuffer_ = GPUPixelContext::GetInstance()
                       ->GetFramebufferFactory()
                       ->CreateFramebuffer(width, height);
  }
}

}  // namespace gpupixel
