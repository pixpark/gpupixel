/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "brightness_filter.h"

USING_NS_GPUPIXEL

REGISTER_FILTER_CLASS(BrightnessFilter)

#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_ANDROID) || defined(GPUPIXEL_MAC)
const std::string kBrightnessFragmentShaderString = R"(
    precision mediump float;
    uniform sampler2D inputImageTexture; uniform lowp float brightness_para;
    varying highp vec2 textureCoordinate;

    void main() {
      lowp vec4 color = texture2D(inputImageTexture, textureCoordinate);
      gl_FragColor = vec4((color.rgb + vec3(brightness_para)), color.a);
    })";
#elif defined(GPUPIXEL_WIN) || defined(GPUPIXEL_MAC) || defined(GPUPIXEL_LINUX)
const std::string kBrightnessFragmentShaderString = R"(
    uniform sampler2D inputImageTexture; uniform float brightness_para;
    varying vec2 textureCoordinate;

    void main() {
      vec4 color = texture2D(inputImageTexture, textureCoordinate);
      gl_FragColor = vec4((color.rgb + vec3(brightness_para)), color.a);
    })";
#endif

std::shared_ptr<BrightnessFilter> BrightnessFilter::create(
    float brightness /* = 0.0*/) {
  auto ret = std::shared_ptr<BrightnessFilter>(new BrightnessFilter());
  if (ret && !ret->init(brightness)) {
    ret.reset();
  }
  return ret;
}

bool BrightnessFilter::init(float brightness) {
  if (!initWithFragmentShaderString(kBrightnessFragmentShaderString)) {
    return false;
  }

  _brightness = 0.01;
  registerProperty("brightness_para", _brightness,
                   "The brightness of filter with range between -1 and 1.",
                   [this](float& brightness) { setBrightness(brightness); });

  return true;
}

void BrightnessFilter::setBrightness(float brightness) {
  _brightness = brightness;
  if (_brightness > 1.0) {
    _brightness = 1.0;
  } else if (_brightness < -1.0) {
    _brightness = -1.0;
  }
}

bool BrightnessFilter::proceed(bool bUpdateTargets, int64_t frameTime) {
  _filterProgram->setUniformValue("brightness_para", _brightness);
  return Filter::proceed(bUpdateTargets, frameTime);
}
