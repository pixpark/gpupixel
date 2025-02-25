/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "halftone_filter.h"

USING_NS_GPUPIXEL

#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_ANDROID)
const std::string kHalftoneFragmentShaderString = R"(
    uniform highp float pixelSize; uniform highp float aspectRatio;

    uniform sampler2D inputImageTexture;
    varying highp vec2 textureCoordinate;

    const highp vec3 W = vec3(0.2125, 0.7154, 0.0721);

    void main() {
      highp vec2 pixelSizeVec = vec2(pixelSize, pixelSize / aspectRatio);
      highp vec2 samplePos =
          floor(textureCoordinate / pixelSizeVec) * pixelSizeVec +
          0.5 * pixelSizeVec;

      highp vec2 textureCoordinateToUse =
          vec2(textureCoordinate.x,
               (textureCoordinate.y * aspectRatio + 0.5 - 0.5 * aspectRatio));
      highp vec2 adjustedSamplePos = vec2(
          samplePos.x, (samplePos.y * aspectRatio + 0.5 - 0.5 * aspectRatio));
      highp float distanceFromSamplePoint =
          distance(adjustedSamplePos, textureCoordinateToUse);
      lowp vec3 sampledColor = texture2D(inputImageTexture, samplePos).rgb;
      highp float dotScaling = 1.0 - dot(sampledColor, W);

      lowp float checkForPresenceWithinDot =
          1.0 - step(distanceFromSamplePoint, (pixelSize * 0.5) * dotScaling);

      gl_FragColor = vec4(vec3(checkForPresenceWithinDot), 1.0);
    })";
#elif defined(GPUPIXEL_MAC) || defined(GPUPIXEL_WIN) || defined(GPUPIXEL_LINUX)
const std::string kHalftoneFragmentShaderString = R"(
    precision mediump float;
    uniform float pixelSize; uniform float aspectRatio;

    uniform sampler2D inputImageTexture;
    varying vec2 textureCoordinate;

    const vec3 W = vec3(0.2125, 0.7154, 0.0721);

    void main() {
      vec2 pixelSizeVec = vec2(pixelSize, pixelSize / aspectRatio);
      vec2 samplePos = floor(textureCoordinate / pixelSizeVec) * pixelSizeVec +
                       0.5 * pixelSizeVec;

      vec2 textureCoordinateToUse =
          vec2(textureCoordinate.x,
               (textureCoordinate.y * aspectRatio + 0.5 - 0.5 * aspectRatio));
      vec2 adjustedSamplePos = vec2(
          samplePos.x, (samplePos.y * aspectRatio + 0.5 - 0.5 * aspectRatio));
      float distanceFromSamplePoint =
          distance(adjustedSamplePos, textureCoordinateToUse);
      vec3 sampledColor = texture2D(inputImageTexture, samplePos).rgb;
      float dotScaling = 1.0 - dot(sampledColor, W);

      float checkForPresenceWithinDot =
          1.0 - step(distanceFromSamplePoint, (pixelSize * 0.5) * dotScaling);

      gl_FragColor = vec4(vec3(checkForPresenceWithinDot), 1.0);
    })";
#endif

std::shared_ptr<HalftoneFilter> HalftoneFilter::create() {
  auto ret = std::shared_ptr<HalftoneFilter>(new HalftoneFilter());
  if (ret && !ret->init()) {
    ret.reset();
  }
  return ret;
}

bool HalftoneFilter::init() {
  if (!initWithFragmentShaderString(kHalftoneFragmentShaderString)) {
    return false;
  }

  setPixelSize(0.01);
  registerProperty(
      "pixelSize", _pixelSize,
      "The size of a pixel that you want to pixellate, ranges from 0 to 0.05.",
      [this](float& pixelSize) { setPixelSize(pixelSize); });

  return true;
}
