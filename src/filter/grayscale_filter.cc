/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "grayscale_filter.h"

NS_GPUPIXEL_BEGIN

#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_ANDROID)
const std::string kGrayscaleFragmentShaderString = R"(
    precision highp float; uniform sampler2D inputImageTexture;
    varying highp vec2 textureCoordinate;

    const highp vec3 W = vec3(0.2125, 0.7154, 0.0721);

    void main() {
      lowp vec4 color = texture2D(inputImageTexture, textureCoordinate);
      float luminance = dot(color.rgb, vec3(0.2125, 0.7154, 0.0721));
      gl_FragColor = vec4(vec3(luminance), color.a);
    })";
#elif defined(GPUPIXEL_MAC) || defined(GPUPIXEL_WIN) || defined(GPUPIXEL_LINUX)
const std::string kGrayscaleFragmentShaderString = R"(
    precision mediump float;
    uniform sampler2D inputImageTexture; varying vec2 textureCoordinate;

    const vec3 W = vec3(0.2125, 0.7154, 0.0721);

    void main() {
      vec4 color = texture2D(inputImageTexture, textureCoordinate);
      float luminance = dot(color.rgb, vec3(0.2125, 0.7154, 0.0721));
      gl_FragColor = vec4(vec3(luminance), color.a);
    })";
#endif

std::shared_ptr<GrayscaleFilter> GrayscaleFilter::create() {
  auto ret = std::shared_ptr<GrayscaleFilter>(new GrayscaleFilter());
  if (!ret->init()) {
    ret.reset();
  }
  return ret;
}

bool GrayscaleFilter::init() {
  if (Filter::initWithFragmentShaderString(kGrayscaleFragmentShaderString)) {
    return true;
  }
  return false;
}

bool GrayscaleFilter::proceed(bool bUpdateTargets, int64_t frameTime) {
  return Filter::proceed(bUpdateTargets, frameTime);
}

NS_GPUPIXEL_END
