/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "contrast_filter.h"

USING_NS_GPUPIXEL

REGISTER_FILTER_CLASS(ContrastFilter)

const std::string kContrastFragmentShaderString = R"(
    precision mediump float;
    uniform sampler2D inputImageTexture; uniform lowp float contrast;
    varying highp vec2 textureCoordinate;

    void main() {
      lowp vec4 color = texture2D(inputImageTexture, textureCoordinate);
      gl_FragColor =
          vec4(((color.rgb - vec3(0.5)) * contrast + vec3(0.5)), color.a);
    })";

std::shared_ptr<ContrastFilter> ContrastFilter::create() {
  auto ret = std::shared_ptr<ContrastFilter>(new ContrastFilter());
  if (ret && !ret->init()) {
    ret.reset();
  }
  return ret;
}

bool ContrastFilter::init() {
  if (!initWithFragmentShaderString(kContrastFragmentShaderString)) {
    return false;
  }

  _contrast = 1.0;
  registerProperty("contrast", _contrast,
                   "The contrast of the image. Contrast ranges from 0.0 to 4.0 "
                   "(max contrast), with 1.0 as the normal level",
                   [this](float& contrast) { setContrast(contrast); });

  return true;
}

void ContrastFilter::setContrast(float contrast) {
  _contrast = contrast;
  if (_contrast > 4.0) {
    _contrast = 4.0;
  } else if (_contrast < 0.0) {
    _contrast = 0.0;
  }
}

bool ContrastFilter::proceed(bool bUpdateTargets, int64_t frameTime) {
  _filterProgram->setUniformValue("contrast", _contrast);
  return Filter::proceed(bUpdateTargets, frameTime);
}
