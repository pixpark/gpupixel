/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "sobel_edge_detection_filter.h"

NS_GPUPIXEL_BEGIN

REGISTER_FILTER_CLASS(SobelEdgeDetectionFilter)

//   Code from "Graphics Shaders: Theory and Practice" by M. Bailey and S.
//   Cunningham
const std::string kSobelEdgeDetectionFragmentShaderString = SHADER_STRING(
    precision mediump float; uniform sampler2D inputImageTexture;
    uniform float edgeStrength;

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
      float h = -topLeftIntensity - 2.0 * topIntensity - topRightIntensity +
                bottomLeftIntensity + 2.0 * bottomIntensity +
                bottomRightIntensity;
      float v = -bottomLeftIntensity - 2.0 * leftIntensity - topLeftIntensity +
                bottomRightIntensity + 2.0 * rightIntensity + topRightIntensity;

      float mag = length(vec2(h, v)) * edgeStrength;
      gl_FragColor = vec4(vec3(mag), 1.0);
    });

SobelEdgeDetectionFilter::SobelEdgeDetectionFilter()
    : _grayscaleFilter(0), _sobelEdgeDetectionFilter(0) {}

SobelEdgeDetectionFilter::~SobelEdgeDetectionFilter() {}

std::shared_ptr<SobelEdgeDetectionFilter> SobelEdgeDetectionFilter::create() {
  auto ret =
      std::shared_ptr<SobelEdgeDetectionFilter>(new SobelEdgeDetectionFilter());
  if (ret && !ret->init()) {
    ret.reset();
  }

  return ret;
}

bool SobelEdgeDetectionFilter::init() {
  if (!FilterGroup::init()) {
    return false;
  }

  _grayscaleFilter = GrayscaleFilter::create();
  _sobelEdgeDetectionFilter = _SobelEdgeDetectionFilter::create();
  _grayscaleFilter->addTarget(_sobelEdgeDetectionFilter);
  addFilter(_grayscaleFilter);

  _edgeStrength = 1.0;
  registerProperty("edgeStrength", _edgeStrength,
                   "The edge strength of sobel edge detection filter",
                   [this](float& edgeStrength) {
                     _sobelEdgeDetectionFilter->setEdgeStrength(edgeStrength);
                   });

  return true;
}

std::shared_ptr<_SobelEdgeDetectionFilter> _SobelEdgeDetectionFilter::create() {
  auto ret = std::shared_ptr<_SobelEdgeDetectionFilter>(
      new _SobelEdgeDetectionFilter());
  if (!ret || !ret->init()) {
    ret.reset();
  }

  return ret;
}

bool _SobelEdgeDetectionFilter::init() {
  if (!initWithFragmentShaderString(kSobelEdgeDetectionFragmentShaderString)) {
    return false;
  }
  _edgeStrength = 1.0;
  return true;
}

void _SobelEdgeDetectionFilter::setEdgeStrength(float edgeStrength) {
  _edgeStrength = edgeStrength;
}

bool _SobelEdgeDetectionFilter::proceed(bool bUpdateTargets,
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

  _filterProgram->setUniformValue("texelWidth", texelWidth);
  _filterProgram->setUniformValue("texelHeight", texelHeight);
  _filterProgram->setUniformValue("edgeStrength", _edgeStrength);
  return NearbySampling3x3Filter::proceed(bUpdateTargets, frameTime);
}

NS_GPUPIXEL_END
