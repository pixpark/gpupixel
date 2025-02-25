/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "color_invert_filter.h"

NS_GPUPIXEL_BEGIN

REGISTER_FILTER_CLASS(ColorInvertFilter)

const std::string kColorInvertFragmentShaderString = R"(
    precision mediump float;
    uniform sampler2D inputImageTexture; varying highp vec2 textureCoordinate;

    void main() {
      lowp vec4 color = texture2D(inputImageTexture, textureCoordinate);
      gl_FragColor = vec4((1.0 - color.rgb), color.a);
    })";

std::shared_ptr<ColorInvertFilter> ColorInvertFilter::create() {
  auto ret = std::shared_ptr<ColorInvertFilter>(new ColorInvertFilter());
  if (ret && !ret->init()) {
    ret.reset();
  }
  return ret;
}

bool ColorInvertFilter::init() {
  if (!Filter::initWithFragmentShaderString(kColorInvertFragmentShaderString)) {
    return false;
  }
  return true;
}

bool ColorInvertFilter::proceed(bool bUpdateTargets, int64_t frameTime) {
  return Filter::proceed(bUpdateTargets, frameTime);
}

NS_GPUPIXEL_END
