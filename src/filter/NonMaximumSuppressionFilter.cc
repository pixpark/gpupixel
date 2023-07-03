/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "NonMaximumSuppressionFilter.h"

NS_GPUPIXEL_BEGIN

const std::string kNonMaximumSuppressionShaderString = SHADER_STRING(
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
      lowp vec4 centerColor = texture2D(inputImageTexture, textureCoordinate);

      // Use a tiebreaker for pixels to the left and immediately above this one
      lowp float multiplier = 1.0 - step(centerColor.r, topIntensity);
      multiplier = multiplier * (1.0 - step(centerColor.r, topLeftIntensity));
      multiplier = multiplier * (1.0 - step(centerColor.r, leftIntensity));
      multiplier =
          multiplier * (1.0 - step(centerColor.r, bottomLeftIntensity));

      lowp float maxValue = max(centerColor.r, bottomIntensity);
      maxValue = max(maxValue, bottomRightIntensity);
      maxValue = max(maxValue, rightIntensity);
      maxValue = max(maxValue, topRightIntensity);

      gl_FragColor = vec4(
          (centerColor.rgb * step(maxValue, centerColor.r) * multiplier), 1.0);
    });

std::shared_ptr<NonMaximumSuppressionFilter>
NonMaximumSuppressionFilter::create() {
  auto ret = std::shared_ptr<NonMaximumSuppressionFilter>(
      new NonMaximumSuppressionFilter());
  if (ret && !ret->init()) {
    ret.reset();
  }
  return ret;
}

bool NonMaximumSuppressionFilter::init() {
  if (initWithFragmentShaderString(kNonMaximumSuppressionShaderString)) {
    return true;
  }
  return false;
}

NS_GPUPIXEL_END
