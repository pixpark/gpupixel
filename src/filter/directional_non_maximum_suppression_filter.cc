/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "directional_non_maximum_suppression_filter.h"

namespace gpupixel {

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
DirectionalNonMaximumSuppressionFilter::Create() {
  auto ret = std::shared_ptr<DirectionalNonMaximumSuppressionFilter>(
      new DirectionalNonMaximumSuppressionFilter());
  if (ret && !ret->Init()) {
    ret.reset();
  }
  return ret;
}

bool DirectionalNonMaximumSuppressionFilter::Init() {
  if (InitWithFragmentShaderString(
          kDirectionalNonmaximumSuppressionFragmentShaderString)) {
    _texelWidthUniform = _filterProgram->GetUniformLocation("texelWidth");
    _texelHeightUniform = _filterProgram->GetUniformLocation("texelWidth");

    _filterProgram->SetUniformValue("upperThreshold", (float)0.5);
    _filterProgram->SetUniformValue("lowerThreshold", (float)0.1);

    return true;
  }
  return false;
}

bool DirectionalNonMaximumSuppressionFilter::DoRender(bool updateSinks) {
  float texelWidth = 1.0 / _framebuffer->GetWidth();
  float texelHeight = 1.0 / _framebuffer->GetHeight();

  std::shared_ptr<GPUPixelFramebuffer> inputFramebuffer =
      input_framebuffers_.begin()->second.frameBuffer;
  RotationMode inputRotation = input_framebuffers_.begin()->second.rotationMode;

  if (rotationSwapsSize(inputRotation)) {
    texelWidth = 1.0 / _framebuffer->GetHeight();
    texelHeight = 1.0 / _framebuffer->GetWidth();
  }

  _filterProgram->SetUniformValue(_texelWidthUniform, texelWidth);
  _filterProgram->SetUniformValue(_texelHeightUniform, texelHeight);

  return Filter::DoRender(updateSinks);
}

}
