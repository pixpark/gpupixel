/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "saturation_filter.h"

USING_NS_GPUPIXEL

const std::string kSaturationFragmentShaderString = R"(
    precision mediump float;
    uniform sampler2D inputImageTexture; uniform lowp float saturation;
    varying highp vec2 textureCoordinate;

    // Values from "Graphics Shaders: Theory and Practice" by Bailey and
    // Cunningham
    const mediump vec3 luminanceWeighting = vec3(0.2125, 0.7154, 0.0721);

    void main() {
      lowp vec4 color = texture2D(inputImageTexture, textureCoordinate);
      lowp float luminance = dot(color.rgb, luminanceWeighting);
      lowp vec3 greyScaleColor = vec3(luminance);

      gl_FragColor = vec4(mix(greyScaleColor, color.rgb, saturation), color.a);
    })";

std::shared_ptr<SaturationFilter> SaturationFilter::create() {
  auto ret = std::shared_ptr<SaturationFilter>(new SaturationFilter());
  if (ret && !ret->init()) {
    ret.reset();
  }
  return ret;
}

bool SaturationFilter::init() {
  if (!initWithFragmentShaderString(kSaturationFragmentShaderString)) {
    return false;
  }

  _saturation = 1.0;
  registerProperty(
      "saturation", _saturation,
      "The saturation of an image. Saturation ranges from 0.0 (fully "
      "desaturated) to 2.0 (max saturation), with 1.0 as the normal level",
      [this](float& saturation) { setSaturation(saturation); });

  return true;
}

void SaturationFilter::setSaturation(float saturation) {
  _saturation = saturation;
  if (_saturation > 2.0) {
    _saturation = 2.0;
  } else if (_saturation < 0.0) {
    _saturation = 0.0;
  }
}

bool SaturationFilter::proceed(bool bUpdateTargets, int64_t frameTime) {
  _filterProgram->setUniformValue("saturation", _saturation);
  return Filter::proceed(bUpdateTargets, frameTime);
}
