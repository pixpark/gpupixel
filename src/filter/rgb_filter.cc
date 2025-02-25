/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "rgb_filter.h"

USING_NS_GPUPIXEL

REGISTER_FILTER_CLASS(RGBFilter)

const std::string kRGBFragmentShaderString = R"(
    precision mediump float;
    uniform sampler2D inputImageTexture; uniform highp float redAdjustment;
    uniform highp float greenAdjustment;
    uniform highp float blueAdjustment;
    varying highp vec2 textureCoordinate;

    void main() {
      lowp vec4 color = texture2D(inputImageTexture, textureCoordinate);
      gl_FragColor = vec4(color.r * redAdjustment, color.g * greenAdjustment,
                          color.b * blueAdjustment, color.a);
    })";

std::shared_ptr<RGBFilter> RGBFilter::create() {
  auto ret = std::shared_ptr<RGBFilter>(new RGBFilter());
  if (ret && !ret->init()) {
    ret.reset();
  }
  return ret;
}

bool RGBFilter::init() {
  if (!initWithFragmentShaderString(kRGBFragmentShaderString)) {
    return false;
  }

  _redAdjustment = 1.0;
  _greenAdjustment = 1.0;
  _blueAdjustment = 1.0;

  registerProperty(
      "redAdjustment", _redAdjustment,
      "The red adjustment of the image.The range is from 0.0 up, with 1.0 as "
      "the default.",
      [this](float& redAdjustment) { setRedAdjustment(redAdjustment); });

  registerProperty(
      "greenAdjustment", _greenAdjustment,
      "The green adjustment of the image.The range is from 0.0 up, with 1.0 as "
      "the default.",
      [this](float& greenAdjustment) { setGreenAdjustment(greenAdjustment); });

  registerProperty(
      "blueAdjustment", _blueAdjustment,
      "The blue adjustment of the image.The range is from 0.0 up, with 1.0 as "
      "the default.",
      [this](float& blueAdjustment) { setBlueAdjustment(blueAdjustment); });

  return true;
}

void RGBFilter::setRedAdjustment(float redAdjustment) {
  _redAdjustment = redAdjustment;
  if (_redAdjustment < 0.0) {
    _redAdjustment = 0.0;
  }
}

void RGBFilter::setGreenAdjustment(float greenAdjustment) {
  _greenAdjustment = greenAdjustment;
  if (_greenAdjustment < 0.0) {
    _greenAdjustment = 0.0;
  }
}

void RGBFilter::setBlueAdjustment(float blueAdjustment) {
  _blueAdjustment = blueAdjustment;
  if (_blueAdjustment < 0.0) {
    _blueAdjustment = 0.0;
  }
}
bool RGBFilter::proceed(bool bUpdateTargets, int64_t frameTime) {
  _filterProgram->setUniformValue("redAdjustment", _redAdjustment);
  _filterProgram->setUniformValue("greenAdjustment", _greenAdjustment);
  _filterProgram->setUniformValue("blueAdjustment", _blueAdjustment);
  return Filter::proceed(bUpdateTargets, frameTime);
}
