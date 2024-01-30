/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "hue_filter.h"
#include "math_toolbox.h"

USING_NS_GPUPIXEL

// Adapted from
// http://stackoverflow.com/questions/9234724/how-to-change-hue-of-a-texture-with-glsl
// - see for code and discussion
const std::string kHueFragmentShaderString = R"(
    precision highp float; uniform sampler2D inputImageTexture;
    uniform mediump float hueAdjustment;
    varying highp vec2 textureCoordinate;
    const highp vec4 kRGBToYPrime = vec4(0.299, 0.587, 0.114, 0.0);
    const highp vec4 kRGBToI = vec4(0.595716, -0.274453, -0.321263, 0.0);
    const highp vec4 kRGBToQ = vec4(0.211456, -0.522591, 0.31135, 0.0);
    const highp vec4 kYIQToR = vec4(1.0, 0.9563, 0.6210, 0.0);
    const highp vec4 kYIQToG = vec4(1.0, -0.2721, -0.6474, 0.0);
    const highp vec4 kYIQToB = vec4(1.0, -1.1070, 1.7046, 0.0);

    void main() {
      // Sample the input pixel
      highp vec4 color = texture2D(inputImageTexture, textureCoordinate);

      // Convert to YIQ
      highp float YPrime = dot(color, kRGBToYPrime);
      highp float I = dot(color, kRGBToI);
      highp float Q = dot(color, kRGBToQ);

      // Calculate the hue and chroma
      highp float hue = atan(Q, I);
      highp float chroma = sqrt(I * I + Q * Q);

      // Make the user's adjustments
      hue += (-hueAdjustment);  // why negative rotation?

      // Convert back to YIQ
      Q = chroma * sin(hue);
      I = chroma * cos(hue);

      // Convert back to RGB
      highp vec4 yIQ = vec4(YPrime, I, Q, 0.0);
      color.r = dot(yIQ, kYIQToR);
      color.g = dot(yIQ, kYIQToG);
      color.b = dot(yIQ, kYIQToB);

      // Save the result
      gl_FragColor = color;
    })";

std::shared_ptr<HueFilter> HueFilter::create() {
  auto ret = std::shared_ptr<HueFilter>(new HueFilter());
  if (ret && !ret->init()) {
    ret.reset();
  }
  return ret;
}

bool HueFilter::init() {
  if (!initWithFragmentShaderString(kHueFragmentShaderString)) {
    return false;
  }

  _hueAdjustment = 90;
  registerProperty(
      "hueAdjustment", _hueAdjustment,
      "The hueAdjustment (in degree) of the image",
      [this](float& hueAdjustment) { setHueAdjustment(hueAdjustment); });

  return true;
}

void HueFilter::setHueAdjustment(float hueAdjustment) {
  // Convert degrees to radians for hue rotation
  _hueAdjustment = fmodf(hueAdjustment, 360.0) * M_PI / 180;
}

bool HueFilter::proceed(bool bUpdateTargets, int64_t frameTime) {
  _filterProgram->setUniformValue("hueAdjustment", _hueAdjustment);
  return Filter::proceed(bUpdateTargets, frameTime);
}
