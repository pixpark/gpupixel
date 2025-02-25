/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "directional_non_maximum_suppression_filter.h"

NS_GPUPIXEL_BEGIN

REGISTER_FILTER_CLASS(DirectionalNonMaximumSuppressionFilter)

#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_ANDROID)
const std::string kDirectionalNonmaximumSuppressionFragmentShaderString =
    R"(
        precision mediump float;

        uniform sampler2D inputImageTexture;
        uniform highp float texelWidth;
        uniform highp float texelHeight;
        uniform mediump float upperThreshold;
        uniform mediump float lowerThreshold;

        varying highp vec2 textureCoordinate;

        void main() {
          vec3 currentGradientAndDirection =
              texture2D(inputImageTexture, textureCoordinate).rgb;
          vec2 gradientDirection =
              ((currentGradientAndDirection.gb * 2.0) - 1.0) *
              vec2(texelWidth, texelHeight);

          float firstSampledGradientMagnitude =
              texture2D(inputImageTexture,
                        textureCoordinate + gradientDirection)
                  .r;
          float secondSampledGradientMagnitude =
              texture2D(inputImageTexture,
                        textureCoordinate - gradientDirection)
                  .r;

          float multiplier = step(firstSampledGradientMagnitude,
                                  currentGradientAndDirection.r);
          multiplier = multiplier * step(secondSampledGradientMagnitude,
                                         currentGradientAndDirection.r);

          float thresholdCompliance = smoothstep(lowerThreshold, upperThreshold,
                                                 currentGradientAndDirection.r);
          multiplier = multiplier * thresholdCompliance;

          gl_FragColor = vec4(multiplier, multiplier, multiplier, 1.0);
        })";
#elif defined(GPUPIXEL_MAC) || defined(GPUPIXEL_WIN) || defined(GPUPIXEL_LINUX)
const std::string kDirectionalNonmaximumSuppressionFragmentShaderString =
    R"(
        precision mediump float;
        uniform sampler2D inputImageTexture; uniform float texelWidth;
        uniform float texelHeight;
        uniform float upperThreshold;
        uniform float lowerThreshold;

        varying vec2 textureCoordinate;

        void main() {
          vec3 currentGradientAndDirection =
              texture2D(inputImageTexture, textureCoordinate).rgb;
          vec2 gradientDirection =
              ((currentGradientAndDirection.gb * 2.0) - 1.0) *
              vec2(texelWidth, texelHeight);

          float firstSampledGradientMagnitude =
              texture2D(inputImageTexture,
                        textureCoordinate + gradientDirection)
                  .r;
          float secondSampledGradientMagnitude =
              texture2D(inputImageTexture,
                        textureCoordinate - gradientDirection)
                  .r;

          float multiplier = step(firstSampledGradientMagnitude,
                                  currentGradientAndDirection.r);
          multiplier = multiplier * step(secondSampledGradientMagnitude,
                                         currentGradientAndDirection.r);

          float thresholdCompliance = smoothstep(lowerThreshold, upperThreshold,
                                                 currentGradientAndDirection.r);
          multiplier = multiplier * thresholdCompliance;

          gl_FragColor = vec4(multiplier, multiplier, multiplier, 1.0);
        })";
#endif

std::shared_ptr<DirectionalNonMaximumSuppressionFilter>
DirectionalNonMaximumSuppressionFilter::create() {
  auto ret = std::shared_ptr<DirectionalNonMaximumSuppressionFilter>(
      new DirectionalNonMaximumSuppressionFilter());
  if (ret && !ret->init()) {
    ret.reset();
  }
  return ret;
}

bool DirectionalNonMaximumSuppressionFilter::init() {
  if (initWithFragmentShaderString(
          kDirectionalNonmaximumSuppressionFragmentShaderString)) {
    _texelWidthUniform = _filterProgram->getUniformLocation("texelWidth");
    _texelHeightUniform = _filterProgram->getUniformLocation("texelWidth");

    _filterProgram->setUniformValue("upperThreshold", (float)0.5);
    _filterProgram->setUniformValue("lowerThreshold", (float)0.1);

    return true;
  }
  return false;
}

bool DirectionalNonMaximumSuppressionFilter::proceed(bool bUpdateTargets,
                                                     int64_t frameTime) {
  float texelWidth = 1.0 / _framebuffer->getWidth();
  float texelHeight = 1.0 / _framebuffer->getHeight();

  std::shared_ptr<Framebuffer> inputFramebuffer =
      _inputFramebuffers.begin()->second.frameBuffer;
  RotationMode inputRotation = _inputFramebuffers.begin()->second.rotationMode;

  if (rotationSwapsSize(inputRotation)) {
    texelWidth = 1.0 / _framebuffer->getHeight();
    texelHeight = 1.0 / _framebuffer->getWidth();
  }

  _filterProgram->setUniformValue(_texelWidthUniform, texelWidth);
  _filterProgram->setUniformValue(_texelHeightUniform, texelHeight);

  return Filter::proceed(bUpdateTargets, frameTime);
}

NS_GPUPIXEL_END
