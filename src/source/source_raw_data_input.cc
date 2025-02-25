/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "source_raw_data_input.h"
#include "gpupixel_context.h"
#include "util.h"
#include "face_detector.h"
USING_NS_GPUPIXEL

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
#elif defined(GPUPIXEL_MAC) || defined(GPUPIXEL_WIN) || \
    defined(GPUPIXEL_LINUX) || defined(__emscripten__)
const std::string kI420FragmentShaderString = R"(
    precision mediump float;
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

std::shared_ptr<SourceRawDataInput> SourceRawDataInput::create() {
  auto sourceRawDataInput =
      std::shared_ptr<SourceRawDataInput>(new SourceRawDataInput());
  if (sourceRawDataInput->init()) {
    return sourceRawDataInput;
  }
  return nullptr;
}

SourceRawDataInput::SourceRawDataInput() {}

SourceRawDataInput::~SourceRawDataInput() {
  GPUPixelContext::getInstance()->runSync(
      [=] { glDeleteTextures(4, _textures); });
}

bool SourceRawDataInput::init() {
  _filterProgram = GLProgram::createByShaderString(kI420VertexShaderString,
                                                   kI420FragmentShaderString);
  GPUPixelContext::getInstance()->setActiveShaderProgram(_filterProgram);

  //
  _filterPositionAttribute = _filterProgram->getAttribLocation("position");
  _filterTexCoordAttribute =
      _filterProgram->getAttribLocation("inputTextureCoordinate");

  _filterProgram->setUniformValue("yTexture", 0);
  _filterProgram->setUniformValue("uTexture", 1);
  _filterProgram->setUniformValue("vTexture", 2);

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

void SourceRawDataInput::uploadBytes(const uint8_t* pixels,
                                     int width,
                                     int height,
                                     int stride,
                                     int64_t ts) {
  GPUPixelContext::getInstance()->runSync([=] {
    if(_face_detector) {
      _face_detector->Detect(pixels, width, height, GPUPIXEL_MODE_FMT_VIDEO,GPUPIXEL_FRAME_TYPE_RGBA8888);
    }
    genTextureWithRGBA(pixels, width, height, stride, ts); 
  });
}

void SourceRawDataInput::setRotation(RotationMode rotation) {
  _rotation = rotation;
}

void SourceRawDataInput::uploadBytes(int width,
                                     int height,
                                     const uint8_t* dataY,
                                     int strideY,
                                     const uint8_t* dataU,
                                     int strideU,
                                     const uint8_t* dataV,
                                     int strideV,
                                     int64_t ts) {
  GPUPixelContext::getInstance()->runSync([=] {
    if(_face_detector) {
      _face_detector->Detect(dataY, width, height, GPUPIXEL_MODE_FMT_VIDEO, GPUPIXEL_FRAME_TYPE_YUVI420);
    }

    genTextureWithI420(width, height, dataY, strideY, dataU, strideU, dataV,
                       strideV, ts);
  });
}

int SourceRawDataInput::genTextureWithI420(int width,
                                           int height,
                                           const uint8_t* dataY,
                                           int strideY,
                                           const uint8_t* dataU,
                                           int strideU,
                                           const uint8_t* dataV,
                                           int strideV,
                                           int64_t ts) {
  if (!_framebuffer || (_framebuffer->getWidth() != width ||
                        _framebuffer->getHeight() != height)) {
    _framebuffer =
        GPUPixelContext::getInstance()->getFramebufferCache()->fetchFramebuffer(
            width, height);
  }

  this->setFramebuffer(_framebuffer, NoRotation);

  GPUPixelContext::getInstance()->setActiveShaderProgram(_filterProgram);
  this->getFramebuffer()->active();

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
                                 _getTexureCoordinate(_rotation)));

  const uint8_t* pixels[3] = {dataY, dataU, dataV};
  const int widths[3] = {width, width / 2, width / 2};
  const int heights[3] = {height, height / 2, height / 2};

  for (int i = 0; i < 3; ++i) {
    glActiveTexture(GL_TEXTURE0 + i);
    glBindTexture(GL_TEXTURE_2D, _textures[i]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, widths[i], heights[i], 0,
                 GL_LUMINANCE, GL_UNSIGNED_BYTE, pixels[i]);
  }

  _filterProgram->setUniformValue("texture_type", 0);
  // draw frame buffer
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  this->getFramebuffer()->inactive();

  Source::proceed(true, ts);
  return 0;
}

int SourceRawDataInput::genTextureWithRGBA(const uint8_t* pixels,
                                           int width,
                                           int height,
                                           int stride,
                                           int64_t ts) {
  if (!_framebuffer || (_framebuffer->getWidth() != stride ||
                        _framebuffer->getHeight() != height)) {
    _framebuffer =
        GPUPixelContext::getInstance()->getFramebufferCache()->fetchFramebuffer(
            stride, height);
  }
  this->setFramebuffer(_framebuffer, NoRotation);

  GLuint texture = _textures[3];
  CHECK_GL(glBindTexture(GL_TEXTURE_2D, texture));

#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_MAC)
  CHECK_GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, stride, height, 0, GL_BGRA,
                        GL_UNSIGNED_BYTE, pixels));
#else
  CHECK_GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, stride, height, 0, GL_RGBA,
                        GL_UNSIGNED_BYTE, pixels));
#endif

  GPUPixelContext::getInstance()->setActiveShaderProgram(_filterProgram);
  this->getFramebuffer()->active();

  GLfloat imageVertices[]{
      -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f,
  };

  _filterProgram->setUniformValue("texture_type", 1);

  CHECK_GL(glEnableVertexAttribArray(_filterPositionAttribute));
  CHECK_GL(glVertexAttribPointer(_filterPositionAttribute, 2, GL_FLOAT, 0, 0,
                                 imageVertices));

  CHECK_GL(glEnableVertexAttribArray(_filterTexCoordAttribute));
  CHECK_GL(glVertexAttribPointer(_filterTexCoordAttribute, 2, GL_FLOAT, 0, 0,
                                 _getTexureCoordinate(_rotation)));

  CHECK_GL(glActiveTexture(GL_TEXTURE4));
  CHECK_GL(glBindTexture(GL_TEXTURE_2D, texture));
  _filterProgram->setUniformValue("inputImageTexture", 4);

  // draw frame buffer
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  this->getFramebuffer()->inactive();

  Source::proceed(true, ts);
  return 0;
}

#ifdef __emscripten__
EMSCRIPTEN_BINDINGS(source_raw_data_input) {
  emscripten::class_<SourceRawDataInputWrapper>("SourceRawDataInput")
      .constructor<>()
      .function("addTarget", &SourceRawDataInputWrapper::addTarget)
      .function("uploadRGBBytes", &SourceRawDataInputWrapper::uploadRGBBytes)
      .function("uploadYUVBytes", &SourceRawDataInputWrapper::uploadYUVBytes);
}
#endif
