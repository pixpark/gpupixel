/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "exposure_filter.h"

USING_NS_GPUPIXEL

const std::string kExposureFragmentShaderString = R"(
    precision mediump float;
    uniform sampler2D inputImageTexture; uniform lowp float exposure;
    varying highp vec2 textureCoordinate;

    void main() {
      lowp vec4 color = texture2D(inputImageTexture, textureCoordinate);
      gl_FragColor = vec4(color.rgb * pow(2.0, exposure), color.a);
    })";

std::shared_ptr<ExposureFilter> ExposureFilter::create() {
  auto ret = std::shared_ptr<ExposureFilter>(new ExposureFilter());
  if (ret && !ret->init()) {
    ret.reset();
  }
  return ret;
}

bool ExposureFilter::init() {
  if (!initWithFragmentShaderString(kExposureFragmentShaderString)) {
    return false;
  }

  _exposure = 0.0;
  registerProperty("exposure", _exposure,
                   "The exposure of the image. Exposure ranges from -10.0 to "
                   "10.0 (max contrast), with 0.0 as the normal level",
                   [this](float& exposure) { setExposure(exposure); });

  return true;
}

void ExposureFilter::setExposure(float exposure) {
  _exposure = exposure;
  if (_exposure > 10.0) {
    _exposure = 10.0;
  } else if (_exposure < -10.0) {
    _exposure = -10.0;
  }
}

bool ExposureFilter::proceed(bool bUpdateTargets, int64_t frameTime) {
  _filterProgram->setUniformValue("exposure", _exposure);
  return Filter::proceed(bUpdateTargets, frameTime);
}
