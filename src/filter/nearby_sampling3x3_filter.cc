/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "nearby_sampling3x3_filter.h"

NS_GPUPIXEL_BEGIN

const std::string kNearbySampling3x3SamplingVertexShaderString = R"(
    precision mediump float;
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

bool NearbySampling3x3Filter::initWithFragmentShaderString(
    const std::string& fragmentShaderSource,
    int inputNumber /* = 1*/) {
  if (Filter::initWithShaderString(kNearbySampling3x3SamplingVertexShaderString,
                                   fragmentShaderSource)) {
    _texelSizeMultiplier = 1.0;
    _texelWidthUniform = _filterProgram->getUniformLocation("texelWidth");
    _texelHeightUniform = _filterProgram->getUniformLocation("texelHeight");

    registerProperty("texelSizeMultiplier", _texelSizeMultiplier, "",
                     [this](float& texelSizeMultiplier) {
                       setTexelSizeMultiplier(texelSizeMultiplier);
                     });

    return true;
  }
  return false;
}

bool NearbySampling3x3Filter::proceed(bool bUpdateTargets, int64_t frameTime) {
  float texelWidth = _texelSizeMultiplier / _framebuffer->getWidth();
  float texelHeight = _texelSizeMultiplier / _framebuffer->getHeight();

  RotationMode inputRotation = _inputFramebuffers.begin()->second.rotationMode;
  if (rotationSwapsSize(inputRotation)) {
    texelWidth = _texelSizeMultiplier / _framebuffer->getHeight();
    texelHeight = _texelSizeMultiplier / _framebuffer->getWidth();
  }

  _filterProgram->setUniformValue(_texelWidthUniform, texelWidth);
  _filterProgram->setUniformValue(_texelHeightUniform, texelHeight);
  return Filter::proceed(bUpdateTargets, frameTime);
}

void NearbySampling3x3Filter::setTexelSizeMultiplier(
    float texelSizeMultiplier) {
  if (texelSizeMultiplier > 0) {
    _texelSizeMultiplier = texelSizeMultiplier;
  }
}

NS_GPUPIXEL_END
