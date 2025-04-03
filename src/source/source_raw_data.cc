/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "source_raw_data.h"
#include "gpupixel_context.h"
#include "util.h"

namespace gpupixel {

const std::string kI420VertexShaderString = R"(
    attribute vec4 position; 
    attribute vec4 inputTextureCoordinate;
    varying vec2 textureCoordinate;

    void main() {
      textureCoordinate = (inputTextureCoordinate).xy;
      gl_Position = position;
    })";

#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_ANDROID)
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
#elif defined(GPUPIXEL_MAC) || defined(GPUPIXEL_WIN) || defined(GPUPIXEL_LINUX)
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
  auto sourceRawDataInput =
      std::shared_ptr<SourceRawData>(new SourceRawData());
  if (sourceRawDataInput->Init()) {
    return sourceRawDataInput;
  }
  return nullptr;
}

SourceRawData::SourceRawData() {}

SourceRawData::~SourceRawData() {
  GPUPixelContext::GetInstance()->RunSync(
      [=] { glDeleteTextures(4, _textures); });
}

bool SourceRawData::Init() {
  _filterProgram = GPUPixelGLProgram::createByShaderString(kI420VertexShaderString,
                                                   kI420FragmentShaderString);
  GPUPixelContext::GetInstance()->SetActiveGlProgram(_filterProgram);

  //
  _filterPositionAttribute = _filterProgram->GetAttribLocation("position");
  _filterTexCoordAttribute =
      _filterProgram->GetAttribLocation("inputTextureCoordinate");

  _filterProgram->SetUniformValue("yTexture", 0);
  _filterProgram->SetUniformValue("uTexture", 1);
  _filterProgram->SetUniformValue("vTexture", 2);

  if (0 == _textures[0]) {
    glGenTextures(4, _textures);
  }

  for (int i = 0; i < 4; ++i) {
    glBindTexture(GL_TEXTURE_2D, _textures[i]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  }

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  return true;
}

void SourceRawData::SetRotation(RotationMode rotation) {
  _rotation = rotation;
}

void SourceRawData::ProcessData(const uint8_t* data,
                   int width,
                   int height,
                   int stride,
                   GPUPIXEL_FRAME_TYPE type) {
                        if(type == GPUPIXEL_FRAME_TYPE_YUVI420) {
                          GPUPixelContext::GetInstance()->RunSync([=] {
                            // 计算YUV各个通道的起始指针和步长
                            const uint8_t* dataY = data; // Y通道起始位置
                            int strideY = width; // Y通道的步长等于宽度
                            
                            // U通道紧跟在Y通道之后，大小为width*height/4
                            const uint8_t* dataU = data + (width * height);
                            int strideU = width / 2; // U通道的步长是宽度的一半
                            
                            // V通道紧跟在U通道之后，大小为width*height/4
                            const uint8_t* dataV = dataU + (width * height / 4);
                            int strideV = width / 2; // V通道的步长是宽度的一半
                            
                            genTextureWithI420(width, height, dataY, strideY, dataU, strideU, dataV,
                                              strideV);
                          });
                        } else {
                            GPUPixelContext::GetInstance()->RunSync([=] {
                          genTextureWithPixels(data, width, height, stride, type);
                            });
                        }
  }


int SourceRawData::genTextureWithI420(int width,
                                           int height,
                                           const uint8_t* dataY,
                                           int strideY,
                                           const uint8_t* dataU,
                                           int strideU,
                                           const uint8_t* dataV,
                                           int strideV) {
  if (!_framebuffer || (_framebuffer->GetWidth() != width ||
                        _framebuffer->GetHeight() != height)) {
    _framebuffer =
        GPUPixelContext::GetInstance()->GetFramebufferFactory()->CreateFramebuffer(
            width, height);
  }

  this->SetFramebuffer(_framebuffer, NoRotation);

  GPUPixelContext::GetInstance()->SetActiveGlProgram(_filterProgram);
  this->GetFramebuffer()->Active();

  GLfloat imageVertices[]{
      -1.0, -1.0,  // left down
      1.0,  -1.0,  // right down
      -1.0, 1.0,   // left up
      1.0,  1.0    // right up
  };

  CHECK_GL(glEnableVertexAttribArray(_filterPositionAttribute));
  CHECK_GL(glVertexAttribPointer(_filterPositionAttribute, 2, GL_FLOAT, 0, 0,
                                 imageVertices));

  CHECK_GL(glEnableVertexAttribArray(_filterTexCoordAttribute));
  CHECK_GL(glVertexAttribPointer(_filterTexCoordAttribute, 2, GL_FLOAT, 0, 0,
                                 GetTexureCoordinate(_rotation)));

  const uint8_t* pixels[3] = {dataY, dataU, dataV};
  const int widths[3] = {width, width / 2, width / 2};
  const int heights[3] = {height, height / 2, height / 2};

  for (int i = 0; i < 3; ++i) {
    glActiveTexture(GL_TEXTURE0 + i);
    glBindTexture(GL_TEXTURE_2D, _textures[i]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, widths[i], heights[i], 0,
                 GL_LUMINANCE, GL_UNSIGNED_BYTE, pixels[i]);
  }
  
  _filterProgram->SetUniformValue("texture_type", 0);
  // draw frame buffer
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  this->GetFramebuffer()->Inactive();

  Source::DoRender(true);
  return 0;
}

int SourceRawData::genTextureWithPixels(const uint8_t* pixels,
                                           int width,
                                           int height,
                                           int stride,
                                           GPUPIXEL_FRAME_TYPE type) {
  if (!_framebuffer || (_framebuffer->GetWidth() != stride ||
                        _framebuffer->GetHeight() != height)) {
    _framebuffer =
        GPUPixelContext::GetInstance()->GetFramebufferFactory()->CreateFramebuffer(
            stride, height);
  }
  this->SetFramebuffer(_framebuffer, NoRotation);

  GLuint texture = _textures[3];
  CHECK_GL(glBindTexture(GL_TEXTURE_2D, texture));

  if (type == GPUPIXEL_FRAME_TYPE_BGRA) {
    #if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_MAC)
    CHECK_GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, stride, height, 0, GL_BGRA,
                          GL_UNSIGNED_BYTE, pixels));
    #endif
  } else if (type == GPUPIXEL_FRAME_TYPE_RGBA) {
    CHECK_GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, stride, height, 0, GL_RGBA,
                          GL_UNSIGNED_BYTE, pixels));
  }

  GPUPixelContext::GetInstance()->SetActiveGlProgram(_filterProgram);
  this->GetFramebuffer()->Active();

  GLfloat imageVertices[]{
    -1.0f, -1.0f,
    1.0f, -1.0f,
    -1.0f, 1.0f,
    1.0f, 1.0f,
  };

  _filterProgram->SetUniformValue("texture_type", 1);

  CHECK_GL(glEnableVertexAttribArray(_filterPositionAttribute));
  CHECK_GL(glVertexAttribPointer(_filterPositionAttribute, 2, GL_FLOAT, 0, 0,
                                 imageVertices));

  CHECK_GL(glEnableVertexAttribArray(_filterTexCoordAttribute));
  CHECK_GL(glVertexAttribPointer(_filterTexCoordAttribute, 2, GL_FLOAT, 0, 0,
                                 GetTexureCoordinate(_rotation)));

  CHECK_GL(glActiveTexture(GL_TEXTURE4));
  CHECK_GL(glBindTexture(GL_TEXTURE_2D, texture));
  _filterProgram->SetUniformValue("inputImageTexture", 4);

  // draw frame buffer
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  this->GetFramebuffer()->Inactive();

  Source::DoRender(true);
  return 0;
}

} // namespace gpupixel