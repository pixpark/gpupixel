/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "color_matrix_filter.h"

NS_GPUPIXEL_BEGIN

#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_ANDROID)
const std::string kColorMatrixFragmentShaderString = R"(
    uniform sampler2D inputImageTexture; uniform lowp mat4 colorMatrix;
    uniform lowp float intensity;

    varying highp vec2 textureCoordinate;

    void main() {
      lowp vec4 textureColor = texture2D(inputImageTexture, textureCoordinate);
      lowp vec4 outputColor = textureColor * colorMatrix;

      gl_FragColor =
          (intensity * outputColor) + ((1.0 - intensity) * textureColor);
    })";
#elif defined(GPUPIXEL_MAC) || defined(GPUPIXEL_WIN) || defined(GPUPIXEL_LINUX)
const std::string kColorMatrixFragmentShaderString = R"(
    precision mediump float;
    uniform sampler2D inputImageTexture; uniform mat4 colorMatrix;
    uniform float intensity;

    varying vec2 textureCoordinate;

    void main() {
      vec4 textureColor = texture2D(inputImageTexture, textureCoordinate);
      vec4 outputColor = textureColor * colorMatrix;

      gl_FragColor =
          (intensity * outputColor) + ((1.0 - intensity) * textureColor);
    })";
#endif

ColorMatrixFilter::ColorMatrixFilter()
    : _intensity(1.0), _colorMatrix(Matrix4::IDENTITY) {}

std::shared_ptr<ColorMatrixFilter> ColorMatrixFilter::create() {
  auto ret = std::shared_ptr<ColorMatrixFilter>(new ColorMatrixFilter());
  if (ret && !ret->init()) {
    ret.reset();
  }
  return ret;
}

bool ColorMatrixFilter::init() {
  if (!Filter::initWithFragmentShaderString(kColorMatrixFragmentShaderString)) {
    return false;
  }

  registerProperty("intensity", _intensity,
                   "The percentage of color applied by color matrix with range "
                   "between 0 and 1.",
                   [this](float& intensity) {
                     if (intensity > 1.0) {
                       intensity = 1.0;
                     } else if (intensity < 0.0) {
                       intensity = 0.0;
                     }
                     setIntensity(1.0);
                   });

  // todo register paoperty of color matrix

  return true;
}

bool ColorMatrixFilter::proceed(bool bUpdateTargets, int64_t frameTime) {
  _filterProgram->setUniformValue("intensity", _intensity);
  _filterProgram->setUniformValue("colorMatrix", _colorMatrix);
  return Filter::proceed(bUpdateTargets, frameTime);
}

NS_GPUPIXEL_END
