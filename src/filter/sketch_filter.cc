/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "sketch_filter.h"
#include "gpupixel_context.h"
namespace gpupixel {

const std::string kSketchFilterFragmentShaderString = R"(
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

      float mag = 1.0 - length(vec2(h, v)) * edgeStrength;
      gl_FragColor = vec4(vec3(mag), 1.0);
    })";

SketchFilter::SketchFilter() : grayscale_filter_(0), sketch_filter_(0) {}

SketchFilter::~SketchFilter() {}

std::shared_ptr<SketchFilter> SketchFilter::Create() {
  auto ret = std::shared_ptr<SketchFilter>(new SketchFilter());
  gpupixel::GPUPixelContext::GetInstance()->SyncRunWithContext([&] {
    if (ret && !ret->Init()) {
      ret.reset();
    }
  });

  return ret;
}

bool SketchFilter::Init() {
  if (!FilterGroup::Init()) {
    return false;
  }

  grayscale_filter_ = GrayscaleFilter::Create();
  sketch_filter_ = _SketchFilter::Create();
  grayscale_filter_->AddSink(sketch_filter_);
  AddFilter(grayscale_filter_);

  edge_strength_ = 1.0;
  RegisterProperty("edgeStrength", edge_strength_,
                   "The edge strength of sobel edge detection filter",
                   [this](float& edgeStrength) {
                     sketch_filter_->setEdgeStrength(edgeStrength);
                   });

  return true;
}

std::shared_ptr<_SketchFilter> _SketchFilter::Create() {
  auto ret = std::shared_ptr<_SketchFilter>(new _SketchFilter());
  if (!ret || !ret->Init()) {
    ret.reset();
  }
  return ret;
}

bool _SketchFilter::Init() {
  if (!InitWithFragmentShaderString(kSketchFilterFragmentShaderString)) {
    return false;
  }
  edge_strength_ = 1.0;
  return true;
}

void _SketchFilter::setEdgeStrength(float edgeStrength) {
  edge_strength_ = edgeStrength;
}

bool _SketchFilter::DoRender(bool updateSinks) {
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

  filter_program_->SetUniformValue("texelWidth", texelWidth);
  filter_program_->SetUniformValue("texelHeight", texelHeight);
  filter_program_->SetUniformValue("edgeStrength", edge_strength_);
  return NearbySampling3x3Filter::DoRender(updateSinks);
}

}  // namespace gpupixel
