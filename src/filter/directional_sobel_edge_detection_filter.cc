/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "directional_sobel_edge_detection_filter.h"

NS_GPUPIXEL_BEGIN

#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_ANDROID)
const std::string kDirectionalSobelEdgeDetectionFragmentShaderString =
    R"(
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
          float topLeftIntensity =
              texture2D(inputImageTexture, vTopLeftTexCoord).r;
          float bottomRightIntensity =
              texture2D(inputImageTexture, vBottomRightTexCoord).r;
          float leftIntensity = texture2D(inputImageTexture, vLeftTexCoord).r;
          float rightIntensity = texture2D(inputImageTexture, vRightTexCoord).r;
          float bottomIntensity =
              texture2D(inputImageTexture, vBottomTexCoord).r;
          float topIntensity = texture2D(inputImageTexture, vTopTexCoord).r;

          vec2 gradientDirection;
          gradientDirection.x = -bottomLeftIntensity - 2.0 * leftIntensity -
                                topLeftIntensity + bottomRightIntensity +
                                2.0 * rightIntensity + topRightIntensity;
          gradientDirection.y = -topLeftIntensity - 2.0 * topIntensity -
                                topRightIntensity + bottomLeftIntensity +
                                2.0 * bottomIntensity + bottomRightIntensity;

          float gradientMagnitude = length(gradientDirection);
          vec2 normalizedDirection = normalize(gradientDirection);
          normalizedDirection =
              sign(normalizedDirection) *
              floor(abs(normalizedDirection) +
                    0.617316);  // Offset by 1-sin(pi/8) to set
                                // to 0 if near axis, 1 if away
          normalizedDirection = (normalizedDirection + 1.0) *
                                0.5;  // Place -1.0 - 1.0 within 0 - 1.0

          gl_FragColor = vec4(gradientMagnitude, normalizedDirection.x,
                              normalizedDirection.y, 1.0);
        })";
#elif defined(GPUPIXEL_MAC) || defined(GPUPIXEL_WIN) || defined(GPUPIXEL_LINUX)
const std::string kDirectionalSobelEdgeDetectionFragmentShaderString =
    R"(
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
          float topLeftIntensity =
              texture2D(inputImageTexture, vTopLeftTexCoord).r;
          float bottomRightIntensity =
              texture2D(inputImageTexture, vBottomRightTexCoord).r;
          float leftIntensity = texture2D(inputImageTexture, vLeftTexCoord).r;
          float rightIntensity = texture2D(inputImageTexture, vRightTexCoord).r;
          float bottomIntensity =
              texture2D(inputImageTexture, vBottomTexCoord).r;
          float topIntensity = texture2D(inputImageTexture, vTopTexCoord).r;

          vec2 gradientDirection;
          gradientDirection.x = -bottomLeftIntensity - 2.0 * leftIntensity -
                                topLeftIntensity + bottomRightIntensity +
                                2.0 * rightIntensity + topRightIntensity;
          gradientDirection.y = -topLeftIntensity - 2.0 * topIntensity -
                                topRightIntensity + bottomLeftIntensity +
                                2.0 * bottomIntensity + bottomRightIntensity;

          float gradientMagnitude = length(gradientDirection);
          vec2 normalizedDirection = normalize(gradientDirection);
          normalizedDirection =
              sign(normalizedDirection) *
              floor(abs(normalizedDirection) +
                    0.617316);  // Offset by 1-sin(pi/8) to set
                                // to 0 if near axis, 1 if away
          normalizedDirection = (normalizedDirection + 1.0) *
                                0.5;  // Place -1.0 - 1.0 within 0 - 1.0

          gl_FragColor = vec4(gradientMagnitude, normalizedDirection.x,
                              normalizedDirection.y, 1.0);
        })";
#endif

std::shared_ptr<DirectionalSobelEdgeDetectionFilter>
DirectionalSobelEdgeDetectionFilter::create() {
  auto ret = std::shared_ptr<DirectionalSobelEdgeDetectionFilter>(
      new DirectionalSobelEdgeDetectionFilter());
  if (ret && !ret->init()) {
    ret.reset();
  }
  return ret;
}

bool DirectionalSobelEdgeDetectionFilter::init() {
  if (initWithFragmentShaderString(
          kDirectionalSobelEdgeDetectionFragmentShaderString)) {
    return true;
  }
  return false;
}

NS_GPUPIXEL_END
