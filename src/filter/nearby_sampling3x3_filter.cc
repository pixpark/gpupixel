/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "nearby_sampling3x3_filter.h"
#include "gpupixel_context.h"
namespace gpupixel {

const std::string kNearbySampling3x3SamplingVertexShaderString = R"(
    attribute vec4 position; attribute vec4 inputTextureCoordinate;

    uniform float texelWidth;
    uniform float texelHeight;

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
      gl_Position = position;

      vec2 widthStep = vec2(texelWidth, 0.0);
      vec2 heightStep = vec2(0.0, texelHeight);
      vec2 widthHeightStep = vec2(texelWidth, texelHeight);
      vec2 widthNegativeHeightStep = vec2(texelWidth, -texelHeight);

      textureCoordinate = inputTextureCoordinate.xy;
      vLeftTexCoord = inputTextureCoordinate.xy - widthStep;
      vRightTexCoord = inputTextureCoordinate.xy + widthStep;

      vTopTexCoord = inputTextureCoordinate.xy - heightStep;
      vTopLeftTexCoord = inputTextureCoordinate.xy - widthHeightStep;
      vTopRightTexCoord = inputTextureCoordinate.xy + widthNegativeHeightStep;

      vBottomTexCoord = inputTextureCoordinate.xy + heightStep;
      vBottomLeftTexCoord = inputTextureCoordinate.xy - widthNegativeHeightStep;
      vBottomRightTexCoord = inputTextureCoordinate.xy + widthHeightStep;
    })";

bool NearbySampling3x3Filter::InitWithFragmentShaderString(
    const std::string& fragmentShaderSource,
    int inputNumber /* = 1*/) {
  if (Filter::InitWithShaderString(kNearbySampling3x3SamplingVertexShaderString,
                                   fragmentShaderSource)) {
    texel_size_multiplier_ = 1.0;
    texel_width_uniform_ = filter_program_->GetUniformLocation("texelWidth");
    texel_height_uniform_ = filter_program_->GetUniformLocation("texelHeight");

    RegisterProperty("texelSizeMultiplier", texel_size_multiplier_, "",
                     [this](float& texelSizeMultiplier) {
                       setTexelSizeMultiplier(texelSizeMultiplier);
                     });

    return true;
  }
  return false;
}

bool NearbySampling3x3Filter::DoRender(bool updateSinks) {
  float texelWidth = texel_size_multiplier_ / framebuffer_->GetWidth();
  float texelHeight = texel_size_multiplier_ / framebuffer_->GetHeight();

  RotationMode inputRotation =
      input_framebuffers_.begin()->second.rotation_mode;
  if (rotationSwapsSize(inputRotation)) {
    texelWidth = texel_size_multiplier_ / framebuffer_->GetHeight();
    texelHeight = texel_size_multiplier_ / framebuffer_->GetWidth();
  }

  filter_program_->SetUniformValue(texel_width_uniform_, texelWidth);
  filter_program_->SetUniformValue(texel_height_uniform_, texelHeight);
  return Filter::DoRender(updateSinks);
}

void NearbySampling3x3Filter::setTexelSizeMultiplier(
    float texelSizeMultiplier) {
  if (texelSizeMultiplier > 0) {
    texel_size_multiplier_ = texelSizeMultiplier;
  }
}

}  // namespace gpupixel
