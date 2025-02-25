/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "luminance_range_filter.h"

USING_NS_GPUPIXEL

#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_ANDROID)
const std::string kLuminanceRangeFragmentShaderString = R"(
    uniform sampler2D inputImageTexture;
    uniform lowp float rangeReductionFactor;
    varying highp vec2 textureCoordinate;

    // Values from "Graphics Shaders: Theory and Practice" by Bailey and
    // Cunningham
    const mediump vec3 luminanceWeighting = vec3(0.2125, 0.7154, 0.0721);

    void main() {
      lowp vec4 color = texture2D(inputImageTexture, textureCoordinate);
      mediump float luminance = dot(color.rgb, luminanceWeighting);
      mediump float luminanceRatio = ((0.5 - luminance) * rangeReductionFactor);
      gl_FragColor = vec4((color.rgb) + (luminanceRatio), color.a);
    })";
#elif defined(GPUPIXEL_MAC) || defined(GPUPIXEL_WIN) || defined(GPUPIXEL_LINUX)
const std::string kLuminanceRangeFragmentShaderString = R"(
    precision mediump float;
    uniform sampler2D inputImageTexture; uniform float rangeReductionFactor;
    varying vec2 textureCoordinate;

    // Values from "Graphics Shaders: Theory and Practice" by Bailey and
    // Cunningham
    const vec3 luminanceWeighting = vec3(0.2125, 0.7154, 0.0721);

    void main() {
      vec4 color = texture2D(inputImageTexture, textureCoordinate);
      float luminance = dot(color.rgb, luminanceWeighting);
      float luminanceRatio = ((0.5 - luminance) * rangeReductionFactor);
      gl_FragColor = vec4((color.rgb) + (luminanceRatio), color.a);
    })";
#endif

std::shared_ptr<LuminanceRangeFilter> LuminanceRangeFilter::create() {
  auto ret = std::shared_ptr<LuminanceRangeFilter>(new LuminanceRangeFilter());
  if (ret && !ret->init()) {
    ret.reset();
  }
  return ret;
}

bool LuminanceRangeFilter::init() {
  if (!initWithFragmentShaderString(kLuminanceRangeFragmentShaderString)) {
    return false;
  }

  _rangeReductionFactor = 0.6;
  registerProperty("rangeReductionFactor", _rangeReductionFactor,
                   "The degree to reduce the luminance range, from 0.0 to 1.0. "
                   "Default is 0.6.",
                   [this](float& rangeReductionFactor) {
                     setRangeReductionFactor(rangeReductionFactor);
                   });

  return true;
}

void LuminanceRangeFilter::setRangeReductionFactor(float rangeReductionFactor) {
  _rangeReductionFactor = rangeReductionFactor;
  if (_rangeReductionFactor > 1.0) {
    _rangeReductionFactor = 1.0;
  } else if (_rangeReductionFactor < 0.0) {
    _rangeReductionFactor = 0.0;
  }
}

bool LuminanceRangeFilter::proceed(bool bUpdateTargets, int64_t frameTime) {
  _filterProgram->setUniformValue("rangeReductionFactor",
                                  _rangeReductionFactor);
  return Filter::proceed(bUpdateTargets, frameTime);
}
