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

const std::string kI420VertexShaderString = R"(
    attribute vec4 position; 
    attribute vec4 inputTextureCoordinate;
    varying vec2 textureCoordinate;

    void main() {
      textureCoordinate = (inputTextureCoordinate).xy;
      gl_Position = position;
    })";

#if defined(GPUPIXEL_GLES_SHADER)
const std::string kI420FragmentShaderString = R"(
    varying mediump vec2 textureCoordinate; uniform sampler2D yTexture;
    uniform sampler2D uTexture;
    uniform sampler2D vTexture;
    uniform sampler2D inputImageTexture;
    uniform int texture_type;
    mediump mat3 trans =
        mat3(1.0, 1.0, 1.0,
             0, -0.34414, 1.772,
             1.402, -0.71414, 0);

    void main() {
      mediump vec3 yuv;
      if (texture_type == 0) {  // yuv
        yuv.x = texture2D(yTexture, textureCoordinate).r;
        yuv.y = texture2D(uTexture, textureCoordinate).r - 0.5;
        yuv.z = texture2D(vTexture, textureCoordinate).r - 0.5;

        gl_FragColor = vec4(trans * yuv, 1.0);
      } else {
        gl_FragColor = texture2D(inputImageTexture, textureCoordinate);
      }
    })";
#elif defined(GPUPIXEL_GL_SHADER)
const std::string kI420FragmentShaderString = R"(
    varying vec2 textureCoordinate; uniform sampler2D yTexture;
    uniform sampler2D uTexture;
    uniform sampler2D vTexture;
    uniform sampler2D inputImageTexture;
    uniform int texture_type;
    mat3 trans = mat3(1.0, 1.0, 1.0,
                      0, -0.34414, 1.772,
                      1.402, -0.71414, 0);

    void main() {
      vec3 yuv;
      if (texture_type == 0) {  // i420
        yuv.x = texture2D(yTexture, textureCoordinate).r;
        yuv.y = texture2D(uTexture, textureCoordinate).r - 0.5;
        yuv.z = texture2D(vTexture, textureCoordinate).r - 0.5;

        gl_FragColor = vec4(trans * yuv, 1.0);
      } else {
        gl_FragColor = texture2D(inputImageTexture, textureCoordinate);
      }
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
      [=] { glDeleteTextures(4, textures_); });
}

bool SourceRawData::Init() {
  filter_program_ = GPUPixelGLProgram::CreateWithShaderString(
      kI420VertexShaderString, kI420FragmentShaderString);
  GPUPixelContext::GetInstance()->SetActiveGlProgram(filter_program_);

  //
  filter_position_attribute_ = filter_program_->GetAttribLocation("position");
  filter_tex_coord_attribute_ =
      filter_program_->GetAttribLocation("inputTextureCoordinate");

  if (0 == textures_[0]) {
    glGenTextures(4, textures_);
  }

  for (int i = 0; i < 4; ++i) {
    glBindTexture(GL_TEXTURE_2D, textures_[i]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  }

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
  GPUPixelContext::GetInstance()->SyncRunWithContext([=] {
    if (type == GPUPIXEL_FRAME_TYPE_YUVI420) {
      // Calculate the starting pointers and strides for each YUV channel
      const uint8_t* dataY = data;  // Y channel start position
      int strideY = width;          // Y channel stride equals width

      // U channel follows right after Y channel, size is width*height/4
      const uint8_t* dataU = data + (width * height);
      int strideU = width / 2;  // U channel stride is half the width

      // V channel follows right after U channel, size is width*height/4
      const uint8_t* dataV = dataU + (width * height / 4);
      int strideV = width / 2;  // V channel stride is half the width

      GenerateTextureWithI420(width, height, dataY, strideY, dataU, strideU,
                              dataV, strideV);

    } else {
      GenerateTextureWithPixels(data, width, height, stride, type);
    }
  });
}

int SourceRawData::GenerateTextureWithI420(int width,
                                           int height,
                                           const uint8_t* dataY,
                                           int strideY,
                                           const uint8_t* dataU,
                                           int strideU,
                                           const uint8_t* dataV,
                                           int strideV) {
  if (!framebuffer_ || (framebuffer_->GetWidth() != width ||
                        framebuffer_->GetHeight() != height)) {
    framebuffer_ = GPUPixelContext::GetInstance()
                       ->GetFramebufferFactory()
                       ->CreateFramebuffer(width, height);
  }

  this->SetFramebuffer(framebuffer_, NoRotation);

  GPUPixelContext::GetInstance()->SetActiveGlProgram(filter_program_);
  this->GetFramebuffer()->Activate();

  float imageVertices[]{
      -1.0, -1.0,  // left down
      1.0,  -1.0,  // right down
      -1.0, 1.0,   // left up
      1.0,  1.0    // right up
  };

  GL_CALL(glEnableVertexAttribArray(filter_position_attribute_));
  GL_CALL(glVertexAttribPointer(filter_position_attribute_, 2, GL_FLOAT, 0, 0,
                                imageVertices));

  GL_CALL(glEnableVertexAttribArray(filter_tex_coord_attribute_));
  GL_CALL(glVertexAttribPointer(filter_tex_coord_attribute_, 2, GL_FLOAT, 0, 0,
                                GetTextureCoordinate(rotation_)));

  filter_program_->SetUniformValue("yTexture", 0);
  filter_program_->SetUniformValue("uTexture", 1);
  filter_program_->SetUniformValue("vTexture", 2);

  const uint8_t* pixels[3] = {dataY, dataU, dataV};
  const int widths[3] = {width, width / 2, width / 2};
  const int heights[3] = {height, height / 2, height / 2};

  for (int i = 0; i < 3; ++i) {
    glActiveTexture(GL_TEXTURE0 + i);
    glBindTexture(GL_TEXTURE_2D, textures_[i]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, widths[i], heights[i], 0,
                 GL_LUMINANCE, GL_UNSIGNED_BYTE, pixels[i]);
  }

  filter_program_->SetUniformValue("texture_type", 0);
  // draw frame buffer
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  this->GetFramebuffer()->Deactivate();

  Source::DoRender(true);
  return 0;
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

  uint32_t texture = textures_[3];

  GL_CALL(glBindTexture(GL_TEXTURE_2D, texture));

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

  filter_program_->SetUniformValue("texture_type", 1);

  GL_CALL(glEnableVertexAttribArray(filter_position_attribute_));
  GL_CALL(glVertexAttribPointer(filter_position_attribute_, 2, GL_FLOAT, 0, 0,
                                imageVertices));

  GL_CALL(glEnableVertexAttribArray(filter_tex_coord_attribute_));
  GL_CALL(glVertexAttribPointer(filter_tex_coord_attribute_, 2, GL_FLOAT, 0, 0,
                                GetTextureCoordinate(rotation_)));

  GL_CALL(glActiveTexture(GL_TEXTURE4));
  GL_CALL(glBindTexture(GL_TEXTURE_2D, texture));
  filter_program_->SetUniformValue("inputImageTexture", 4);

  // draw frame buffer
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  this->GetFramebuffer()->Deactivate();

  Source::DoRender(true);
  return 0;
}

}  // namespace gpupixel
