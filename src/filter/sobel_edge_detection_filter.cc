/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "sobel_edge_detection_filter.h"

namespace gpupixel {

REGISTER_FILTER_CLASS(SobelEdgeDetectionFilter)

//   Code from "Graphics Shaders: Theory and Practice" by M. Bailey and S.
//   Cunningham
const std::string kSobelEdgeDetectionFragmentShaderString = R"(
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
    })";

SobelEdgeDetectionFilter::SobelEdgeDetectionFilter()
    : _grayscaleFilter(0), _sobelEdgeDetectionFilter(0) {}

SobelEdgeDetectionFilter::~SobelEdgeDetectionFilter() {}

std::shared_ptr<SobelEdgeDetectionFilter> SobelEdgeDetectionFilter::Create() {
  auto ret =
      std::shared_ptr<SobelEdgeDetectionFilter>(new SobelEdgeDetectionFilter());
  if (ret && !ret->Init()) {
    ret.reset();
  }

  return ret;
}

bool SobelEdgeDetectionFilter::Init() {
  if (!FilterGroup::Init()) {
    return false;
  }

  _grayscaleFilter = GrayscaleFilter::Create();
  _sobelEdgeDetectionFilter = _SobelEdgeDetectionFilter::Create();
  _grayscaleFilter->AddSink(_sobelEdgeDetectionFilter);
  addFilter(_grayscaleFilter);

  _edgeStrength = 1.0;
  RegisterProperty("edgeStrength", _edgeStrength,
                   "The edge strength of sobel edge detection filter",
                   [this](float& edgeStrength) {
                     _sobelEdgeDetectionFilter->setEdgeStrength(edgeStrength);
                   });

  return true;
}

std::shared_ptr<_SobelEdgeDetectionFilter> _SobelEdgeDetectionFilter::Create() {
  auto ret = std::shared_ptr<_SobelEdgeDetectionFilter>(
      new _SobelEdgeDetectionFilter());
  if (!ret || !ret->Init()) {
    ret.reset();
  }

  return ret;
}

bool _SobelEdgeDetectionFilter::Init() {
  if (!InitWithFragmentShaderString(kSobelEdgeDetectionFragmentShaderString)) {
    return false;
  }
  _edgeStrength = 1.0;
  return true;
}

void _SobelEdgeDetectionFilter::setEdgeStrength(float edgeStrength) {
  _edgeStrength = edgeStrength;
}

bool _SobelEdgeDetectionFilter::DoRender(bool updateSinks) {
  float texelWidth = 1.0 / _framebuffer->GetWidth();
  float texelHeight = 1.0 / _framebuffer->GetHeight();

  std::shared_ptr<GPUPixelFramebuffer> inputFramebuffer =
      input_framebuffers_.begin()->second.frameBuffer;
  RotationMode inputRotation = input_framebuffers_.begin()->second.rotationMode;
  if (rotationSwapsSize(inputRotation)) {
    texelWidth = 1.0 / _framebuffer->GetHeight();
    texelHeight = 1.0 / _framebuffer->GetWidth();
  }

  _filterProgram->SetUniformValue("texelWidth", texelWidth);
  _filterProgram->SetUniformValue("texelHeight", texelHeight);
  _filterProgram->SetUniformValue("edgeStrength", _edgeStrength);
  return NearbySampling3x3Filter::DoRender(updateSinks);
}

}
