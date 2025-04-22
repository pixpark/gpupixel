/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "gpupixel/filter/directional_non_maximum_suppression_filter.h"
#include "core/gpupixel_context.h"
#include "utils/util.h"
namespace gpupixel {

#if defined(GPUPIXEL_GLES_SHADER)
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
#elif defined(GPUPIXEL_GL_SHADER)
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
  gpupixel::GPUPixelContext::GetInstance()->SyncRunWithContext([&] {
    if (ret && !ret->Init()) {
      ret.reset();
    }
  });
  return ret;
}

bool DirectionalNonMaximumSuppressionFilter::Init() {
  if (InitWithFragmentShaderString(
          kDirectionalNonmaximumSuppressionFragmentShaderString)) {
    texel_width_uniform_ = filter_program_->GetUniformLocation("texelWidth");
    texel_height_uniform_ = filter_program_->GetUniformLocation("texelHeight");

    filter_program_->SetUniformValue("upperThreshold", (float)0.5);
    filter_program_->SetUniformValue("lowerThreshold", (float)0.1);

    return true;
  }
  return false;
}

bool DirectionalNonMaximumSuppressionFilter::DoRender(bool updateSinks) {
  float texelWidth = 1.0 / framebuffer_->GetWidth();
  float texelHeight = 1.0 / framebuffer_->GetHeight();

  std::shared_ptr<GPUPixelFramebuffer> inputFramebuffer =
      input_framebuffers_.begin()->second.frame_buffer;
  RotationMode inputRotation =
      input_framebuffers_.begin()->second.rotation_mode;

  if (rotationSwapsSize(inputRotation)) {
    texelWidth = 1.0 / framebuffer_->GetHeight();
    texelHeight = 1.0 / framebuffer_->GetWidth();
  }

  filter_program_->SetUniformValue(texel_width_uniform_, texelWidth);
  filter_program_->SetUniformValue(texel_height_uniform_, texelHeight);

  return Filter::DoRender(updateSinks);
}

}  // namespace gpupixel
