/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "weak_pixel_inclusion_filter.h"

NS_GPUPIXEL_BEGIN

REGISTER_FILTER_CLASS(WeakPixelInclusionFilter)

#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_ANDROID)
const std::string kWeakPixelInclusionFragmentShaderString = R"(
    precision mediump float; uniform sampler2D inputImageTexture;

    varying vec2 textureCoordinate;
    varying vec2 vLeftTexCoord;
    varying vec2 vRightTexCoord;

    varying vec2 vTopTexCoord;
    varying vec2 vTopLeftTexCoord;
    varying vec2 vTopRightTexCoord;

    varying vec2 vBottomTexCoord;
    varying vec2 vBottomLeftTexCoord;
    varying vec2 vBottomRightTexCoord;

    void main() {
      float bottomLeftIntensity =
          texture2D(inputImageTexture, vBottomLeftTexCoord).r;
      float topRightIntensity =
          texture2D(inputImageTexture, vTopRightTexCoord).r;
      float topLeftIntensity = texture2D(inputImageTexture, vTopLeftTexCoord).r;
      float bottomRightIntensity =
          texture2D(inputImageTexture, vBottomRightTexCoord).r;
      float leftIntensity = texture2D(inputImageTexture, vLeftTexCoord).r;
      float rightIntensity = texture2D(inputImageTexture, vRightTexCoord).r;
      float bottomIntensity = texture2D(inputImageTexture, vBottomTexCoord).r;
      float topIntensity = texture2D(inputImageTexture, vTopTexCoord).r;
      float centerIntensity = texture2D(inputImageTexture, textureCoordinate).r;

      float pixelIntensitySum =
          bottomLeftIntensity + topRightIntensity + topLeftIntensity +
          bottomRightIntensity + leftIntensity + rightIntensity +
          bottomIntensity + topIntensity + centerIntensity;
      float sumTest = step(1.5, pixelIntensitySum);
      float pixelTest = step(0.01, centerIntensity);

      gl_FragColor = vec4(vec3(sumTest * pixelTest), 1.0);
    })";
#elif defined(GPUPIXEL_MAC) || defined(GPUPIXEL_WIN) || defined(GPUPIXEL_LINUX)
const std::string kWeakPixelInclusionFragmentShaderString = R"(
    precision mediump float;
    uniform sampler2D inputImageTexture;

    varying vec2 textureCoordinate;
    varying vec2 vLeftTexCoord;
    varying vec2 vRightTexCoord;

    varying vec2 vTopTexCoord;
    varying vec2 vTopLeftTexCoord;
    varying vec2 vTopRightTexCoord;

    varying vec2 vBottomTexCoord;
    varying vec2 vBottomLeftTexCoord;
    varying vec2 vBottomRightTexCoord;

    void main() {
      float bottomLeftIntensity =
          texture2D(inputImageTexture, vBottomLeftTexCoord).r;
      float topRightIntensity =
          texture2D(inputImageTexture, vTopRightTexCoord).r;
      float topLeftIntensity = texture2D(inputImageTexture, vTopLeftTexCoord).r;
      float bottomRightIntensity =
          texture2D(inputImageTexture, vBottomRightTexCoord).r;
      float leftIntensity = texture2D(inputImageTexture, vLeftTexCoord).r;
      float rightIntensity = texture2D(inputImageTexture, vRightTexCoord).r;
      float bottomIntensity = texture2D(inputImageTexture, vBottomTexCoord).r;
      float topIntensity = texture2D(inputImageTexture, vTopTexCoord).r;
      float centerIntensity = texture2D(inputImageTexture, textureCoordinate).r;

      float pixelIntensitySum =
          bottomLeftIntensity + topRightIntensity + topLeftIntensity +
          bottomRightIntensity + leftIntensity + rightIntensity +
          bottomIntensity + topIntensity + centerIntensity;
      float sumTest = step(1.5, pixelIntensitySum);
      float pixelTest = step(0.01, centerIntensity);

      gl_FragColor = vec4(vec3(sumTest * pixelTest), 1.0);
    })";
#endif

std::shared_ptr<WeakPixelInclusionFilter> WeakPixelInclusionFilter::create() {
  auto ret =
      std::shared_ptr<WeakPixelInclusionFilter>(new WeakPixelInclusionFilter());
  if (ret && !ret->init()) {
    ret.reset();
  }
  return ret;
}

bool WeakPixelInclusionFilter::init() {
  if (initWithFragmentShaderString(kWeakPixelInclusionFragmentShaderString)) {
    return true;
  }
  return false;
}

NS_GPUPIXEL_END
