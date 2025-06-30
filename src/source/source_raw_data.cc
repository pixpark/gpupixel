/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "gpupixel/source/source_raw_data.h"
#include "core/gpupixel_context.h"
#include "utils/util.h"

namespace gpupixel {

const std::string kVertexShaderString = R"(
    attribute vec4 position; 
    attribute vec4 inputTextureCoordinate;
    varying vec2 textureCoordinate;

    void main() {
      textureCoordinate = (inputTextureCoordinate).xy;
      gl_Position = position;
    })";

#if defined(GPUPIXEL_GLES_SHADER)
const std::string kFragmentShaderString = R"(
    varying mediump vec2 textureCoordinate;
    uniform sampler2D inputImageTexture;

    void main() {
      gl_FragColor = texture2D(inputImageTexture, textureCoordinate);
    })";
#elif defined(GPUPIXEL_GL_SHADER)
const std::string kFragmentShaderString = R"(
    varying vec2 textureCoordinate;
    uniform sampler2D inputImageTexture;

    void main() {
      gl_FragColor = texture2D(inputImageTexture, textureCoordinate);
    })";
#endif

std::shared_ptr<SourceRawData> SourceRawData::Create() {
  auto ret = std::shared_ptr<SourceRawData>(new SourceRawData());
  gpupixel::GPUPixelContext::GetInstance()->SyncRunWithContext([&] {
    if (!ret->Init()) {
      return ret.reset();
    }
  });
  return ret;
}

SourceRawData::SourceRawData() {}

SourceRawData::~SourceRawData() {
  GPUPixelContext::GetInstance()->SyncRunWithContext(
      [=] { glDeleteTextures(1, &texture_); });
}

bool SourceRawData::Init() {
  filter_program_ = GPUPixelGLProgram::CreateWithShaderString(
      kVertexShaderString, kFragmentShaderString);
  GPUPixelContext::GetInstance()->SetActiveGlProgram(filter_program_);

  //
  filter_position_attribute_ = filter_program_->GetAttribLocation("position");
  filter_tex_coord_attribute_ =
      filter_program_->GetAttribLocation("inputTextureCoordinate");

  if (0 == texture_) {
    glGenTextures(1, &texture_);
  }

  glBindTexture(GL_TEXTURE_2D, texture_);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  return true;
}

void SourceRawData::SetRotation(RotationMode rotation) {
  rotation_ = rotation;
}

void SourceRawData::ProcessData(const uint8_t* data,
                                int width,
                                int height,
                                int stride,
                                GPUPIXEL_FRAME_TYPE type) {
  GPUPixelContext::GetInstance()->SyncRunWithContext(
      [=] { GenerateTextureWithPixels(data, width, height, stride, type); });
}

int SourceRawData::GenerateTextureWithPixels(const uint8_t* pixels,
                                             int width,
                                             int height,
                                             int stride,
                                             GPUPIXEL_FRAME_TYPE type) {
  if (!framebuffer_ || (framebuffer_->GetWidth() != stride / 4 ||
                        framebuffer_->GetHeight() != height)) {
    framebuffer_ = GPUPixelContext::GetInstance()
                       ->GetFramebufferFactory()
                       ->CreateFramebuffer(stride / 4, height);
  }
  this->SetFramebuffer(framebuffer_, NoRotation);

  GL_CALL(glBindTexture(GL_TEXTURE_2D, texture_));

  if (type == GPUPIXEL_FRAME_TYPE_BGRA) {
#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_MAC)
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, stride / 4, height, 0,
                         GL_BGRA, GL_UNSIGNED_BYTE, pixels));
#endif
  } else if (type == GPUPIXEL_FRAME_TYPE_RGBA) {
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, stride / 4, height, 0,
                         GL_RGBA, GL_UNSIGNED_BYTE, pixels));
  }

  GPUPixelContext::GetInstance()->SetActiveGlProgram(filter_program_);
  this->GetFramebuffer()->Activate();

  float imageVertices[]{
      -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f,
  };

  GL_CALL(glEnableVertexAttribArray(filter_position_attribute_));
  GL_CALL(glVertexAttribPointer(filter_position_attribute_, 2, GL_FLOAT, 0, 0,
                                imageVertices));

  GL_CALL(glEnableVertexAttribArray(filter_tex_coord_attribute_));
  GL_CALL(glVertexAttribPointer(filter_tex_coord_attribute_, 2, GL_FLOAT, 0, 0,
                                GetTextureCoordinate(rotation_)));

  GL_CALL(glActiveTexture(GL_TEXTURE0));
  GL_CALL(glBindTexture(GL_TEXTURE_2D, texture_));
  filter_program_->SetUniformValue("inputImageTexture", 0);

  // draw frame buffer
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  this->GetFramebuffer()->Deactivate();

  Source::DoRender(true);
  return 0;
}

}  // namespace gpupixel
