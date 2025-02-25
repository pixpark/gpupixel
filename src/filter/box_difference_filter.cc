/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "box_difference_filter.h"
#include "gpupixel_context.h"

NS_GPUPIXEL_BEGIN

const std::string kBoxDifferenceVertexShaderString = R"(
    attribute vec4 position; attribute vec4 inputTextureCoordinate;
    attribute vec4 inputTextureCoordinate2;

    varying vec2 textureCoordinate;
    varying vec2 textureCoordinate2;

    void main() {
      gl_Position = position;
      textureCoordinate = inputTextureCoordinate.xy;
      textureCoordinate2 = inputTextureCoordinate2.xy;
    })";
#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_ANDROID)
const std::string kBoxDifferenceFragmentShaderString = R"(
    varying highp vec2 textureCoordinate; varying highp vec2 textureCoordinate2;
    uniform sampler2D inputImageTexture;
    uniform sampler2D inputImageTexture2;
    uniform highp float delta;

    void main() {
      lowp vec3 iColor = texture2D(inputImageTexture, textureCoordinate).rgb;
      lowp vec3 meanColor =
          texture2D(inputImageTexture2, textureCoordinate2).rgb;
      highp vec3 diffColor = (iColor - meanColor) * delta;
      diffColor = min(diffColor * diffColor, 1.0);
      gl_FragColor = vec4(diffColor, 1.0);
    })";
#elif defined(GPUPIXEL_MAC) || defined(GPUPIXEL_WIN) || defined(GPUPIXEL_LINUX)
const std::string kBoxDifferenceFragmentShaderString = R"(
    precision mediump float;
    varying vec2 textureCoordinate; varying vec2 textureCoordinate2;
    uniform sampler2D inputImageTexture;
    uniform sampler2D inputImageTexture2;
    uniform float delta;

    void main() {
      vec3 iColor = texture2D(inputImageTexture, textureCoordinate).rgb;
      vec3 meanColor = texture2D(inputImageTexture2, textureCoordinate2).rgb;
      vec3 diffColor = (iColor - meanColor) * delta;
      diffColor = min(diffColor * diffColor, 1.0);
      gl_FragColor = vec4(diffColor, 1.0);
    })";
#endif

BoxDifferenceFilter::BoxDifferenceFilter() {}

BoxDifferenceFilter::~BoxDifferenceFilter() {}

std::shared_ptr<BoxDifferenceFilter> BoxDifferenceFilter::create() {
  auto ret = std::shared_ptr<BoxDifferenceFilter>(new BoxDifferenceFilter());
  if (ret && !ret->init()) {
    ret.reset();
  }
  return ret;
}

bool BoxDifferenceFilter::init() {
  if (!Filter::initWithShaderString(kBoxDifferenceVertexShaderString,
                                    kBoxDifferenceFragmentShaderString, 2)) {
    return false;
  }

  // texcoord attribute
  filterTexCoordAttribute_ =
      _filterProgram->getAttribLocation("inputTextureCoordinate");
  filterTexCoordAttribute2_ =
      _filterProgram->getAttribLocation("inputTextureCoordinate2");

  setDelta(7.07);
  return true;
}

bool BoxDifferenceFilter::proceed(bool bUpdateTargets, int64_t frameTime) {
  static const GLfloat imageVertices[] = {
      -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f,
  };

  GPUPixelContext::getInstance()->setActiveShaderProgram(_filterProgram);
  _framebuffer->active();
  CHECK_GL(glClearColor(_backgroundColor.r, _backgroundColor.g,
                        _backgroundColor.b, _backgroundColor.a));
  CHECK_GL(glClear(GL_COLOR_BUFFER_BIT));

  // Texture 0
  CHECK_GL(glActiveTexture(GL_TEXTURE0));
  CHECK_GL(glBindTexture(GL_TEXTURE_2D,
                         _inputFramebuffers[0].frameBuffer->getTexture()));
  _filterProgram->setUniformValue("inputImageTexture", 0);

  // Texture 1
  CHECK_GL(glActiveTexture(GL_TEXTURE1));
  CHECK_GL(glBindTexture(GL_TEXTURE_2D,
                         _inputFramebuffers[1].frameBuffer->getTexture()));
  _filterProgram->setUniformValue("inputImageTexture2", 1);

  CHECK_GL(glEnableVertexAttribArray(filterTexCoordAttribute_));
  CHECK_GL(glVertexAttribPointer(
      filterTexCoordAttribute_, 2, GL_FLOAT, 0, 0,
      _getTexureCoordinate(_inputFramebuffers[0].rotationMode)));

  CHECK_GL(glEnableVertexAttribArray(filterTexCoordAttribute2_));
  CHECK_GL(glVertexAttribPointer(
      filterTexCoordAttribute2_, 2, GL_FLOAT, 0, 0,
      _getTexureCoordinate(_inputFramebuffers[1].rotationMode)));

  // vertex position
  CHECK_GL(glVertexAttribPointer(_filterPositionAttribute, 2, GL_FLOAT, 0, 0,
                                 imageVertices));

  // update uniform
  _filterProgram->setUniformValue("delta", delta_);

  // draw
  CHECK_GL(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));

  _framebuffer->inactive();

  return Source::proceed(bUpdateTargets, frameTime);
}

void BoxDifferenceFilter::setDelta(float delta) {
  this->delta_ = delta;
}
NS_GPUPIXEL_END
