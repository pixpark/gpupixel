/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "beauty_face_filter.h"

namespace gpupixel {

BeautyFaceFilter::BeautyFaceFilter() {}

BeautyFaceFilter::~BeautyFaceFilter() {}

std::shared_ptr<BeautyFaceFilter> BeautyFaceFilter::create() {
  auto ret = std::shared_ptr<BeautyFaceFilter>(new BeautyFaceFilter());
  if (ret && !ret->init()) {
    ret.reset();
  }
  return ret;
}

bool BeautyFaceFilter::init() {
  if (!FilterGroup::init()) {
    return false;
  }

  boxBlurFilter = BoxBlurFilter::create();
  addFilter(boxBlurFilter);

  boxHighPassFilter = BoxHighPassFilter::create();
  addFilter(boxHighPassFilter);

  beautyFilter = BeautyFaceUnitFilter::create();
  addFilter(beautyFilter);

  boxBlurFilter->addSink(beautyFilter, 1);
  boxHighPassFilter->addSink(beautyFilter, 2);

  setTerminalFilter(beautyFilter);

  boxBlurFilter->setTexelSpacingMultiplier(4);
  setRadius(4);

  registerProperty("whiteness", 0, "The whiteness of filter with range between -1 and 1.", [this](float& val) {
      setWhite(val);
  });

  registerProperty("skin_smoothing", 0, "The smoothing of filter with range between -1 and 1.", [this](float& val) {
      setBlurAlpha(val);
  });
  return true;
}

void BeautyFaceFilter::setInputFramebuffer(
    std::shared_ptr<GPUPixelFramebuffer> framebuffer,
    RotationMode rotationMode /* = NoRotation*/,
    int texIdx /* = 0*/) {
  for (auto& filter : _filters) {
    filter->setInputFramebuffer(framebuffer, rotationMode, texIdx);
  }
}

void BeautyFaceFilter::setHighPassDelta(float highPassDelta) {
  boxHighPassFilter->setDelta(highPassDelta);
}

void BeautyFaceFilter::setSharpen(float sharpen) {
  beautyFilter->setSharpen(sharpen);
}

void BeautyFaceFilter::setBlurAlpha(float blurAlpha) {
  beautyFilter->setBlurAlpha(blurAlpha);
}

void BeautyFaceFilter::setWhite(float white) {
  beautyFilter->setWhite(white);
}

void BeautyFaceFilter::setRadius(float radius) {
  boxBlurFilter->setRadius(radius);
  boxHighPassFilter->setRadius(radius);
}
}
