/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "pixellation_filter.h"

USING_NS_GPUPIXEL

REGISTER_FILTER_CLASS(PixellationFilter)

const std::string kPixellationFragmentShaderString = R"(
    precision mediump float;
    uniform highp float pixelSize; uniform highp float aspectRatio;

    uniform sampler2D inputImageTexture;
    varying highp vec2 textureCoordinate;

    void main() {
      highp vec2 pixelSizeVec = vec2(pixelSize, pixelSize / aspectRatio);
      highp vec2 samplePos =
          floor(textureCoordinate / pixelSizeVec) * pixelSizeVec +
          0.5 * pixelSizeVec;
      gl_FragColor = texture2D(inputImageTexture, samplePos);
    })";

std::shared_ptr<PixellationFilter> PixellationFilter::create() {
  auto ret = std::shared_ptr<PixellationFilter>(new PixellationFilter());
  if (ret && !ret->init()) {
    ret.reset();
  }
  return ret;
}

bool PixellationFilter::init() {
  if (!initWithFragmentShaderString(kPixellationFragmentShaderString)) {
    return false;
  }

  _pixelSize = 0.1;
  registerProperty(
      "pixelSize", _pixelSize,
      "The size of a pixel that you want to pixellate, ranges from 0 to 1.",
      [this](float& pixelSize) { setPixelSize(pixelSize); });

  return true;
}

void PixellationFilter::setPixelSize(float pixelSize) {
  _pixelSize = pixelSize;
  if (_pixelSize > 1.0) {
    _pixelSize = 1.0;
  } else if (_pixelSize < 0.0) {
    _pixelSize = 0.0;
  }
}

bool PixellationFilter::proceed(bool bUpdateTargets, int64_t frameTime) {
  float aspectRatio = 1.0;
  std::shared_ptr<Framebuffer> firstInputFramebuffer =
      _inputFramebuffers.begin()->second.frameBuffer;
  aspectRatio = firstInputFramebuffer->getHeight() /
                (float)(firstInputFramebuffer->getWidth());
  _filterProgram->setUniformValue("aspectRatio", aspectRatio);

  float pixelSize = _pixelSize;
  float singlePixelWidth = 1.0 / firstInputFramebuffer->getWidth();
  if (pixelSize < singlePixelWidth) {
    pixelSize = singlePixelWidth;
  }
  _filterProgram->setUniformValue("pixelSize", pixelSize);

  return Filter::proceed(bUpdateTargets, frameTime);
}
