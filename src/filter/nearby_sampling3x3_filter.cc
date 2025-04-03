/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "nearby_sampling3x3_filter.h"

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
    _texelSizeMultiplier = 1.0;
    _texelWidthUniform = _filterProgram->GetUniformLocation("texelWidth");
    _texelHeightUniform = _filterProgram->GetUniformLocation("texelHeight");

    RegisterProperty("texelSizeMultiplier", _texelSizeMultiplier, "",
                     [this](float& texelSizeMultiplier) {
                       setTexelSizeMultiplier(texelSizeMultiplier);
                     });

    return true;
  }
  return false;
}

bool NearbySampling3x3Filter::DoRender(bool updateSinks) {
  float texelWidth = _texelSizeMultiplier / _framebuffer->GetWidth();
  float texelHeight = _texelSizeMultiplier / _framebuffer->GetHeight();

  RotationMode inputRotation = input_framebuffers_.begin()->second.rotationMode;
  if (rotationSwapsSize(inputRotation)) {
    texelWidth = _texelSizeMultiplier / _framebuffer->GetHeight();
    texelHeight = _texelSizeMultiplier / _framebuffer->GetWidth();
  }

  _filterProgram->SetUniformValue(_texelWidthUniform, texelWidth);
  _filterProgram->SetUniformValue(_texelHeightUniform, texelHeight);
  return Filter::DoRender(updateSinks);
}

void NearbySampling3x3Filter::setTexelSizeMultiplier(
    float texelSizeMultiplier) {
  if (texelSizeMultiplier > 0) {
    _texelSizeMultiplier = texelSizeMultiplier;
  }
}

}
