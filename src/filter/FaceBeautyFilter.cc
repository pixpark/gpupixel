/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "FaceBeautyFilter.h"

NS_GPUPIXEL_BEGIN

FaceBeautyFilter::FaceBeautyFilter() {}

FaceBeautyFilter::~FaceBeautyFilter() {}

std::shared_ptr<FaceBeautyFilter> FaceBeautyFilter::create() {
  auto ret = std::shared_ptr<FaceBeautyFilter>(new FaceBeautyFilter());
  if (ret && !ret->init()) {
    ret.reset();
  }
  return ret;
}

bool FaceBeautyFilter::init() {
  if (!FilterGroup::init()) {
    return false;
  }

  boxBlurFilter = BoxBlurFilter::create();
  addFilter(boxBlurFilter);

  boxHighPassFilter = BoxHighPassFilter::create();
  addFilter(boxHighPassFilter);

  beautyFilter = BaseBeautyFaceFilter::create();
  addFilter(beautyFilter);

  boxBlurFilter->addTarget(beautyFilter, 1);
  boxHighPassFilter->addTarget(beautyFilter, 2);

  setTerminalFilter(beautyFilter);

  boxBlurFilter->setTexelSpacingMultiplier(4);

  setRadius(4);

  //    setBlurAlpha(0.5);
  //    setWhite(0.2);
  //    setSharpen(0.5);
  return true;
}

void FaceBeautyFilter::setInputFramebuffer(
    std::shared_ptr<Framebuffer> framebuffer,
    RotationMode rotationMode /* = NoRotation*/,
    int texIdx /* = 0*/) {
  for (auto& filter : _filters) {
    filter->setInputFramebuffer(framebuffer, rotationMode, texIdx);
  }
}

void FaceBeautyFilter::setHighPassDelta(float highPassDelta) {
  boxHighPassFilter->setDelta(highPassDelta);
}

void FaceBeautyFilter::setSharpen(float sharpen) {
  beautyFilter->setSharpen(sharpen);
}

void FaceBeautyFilter::setBlurAlpha(float blurAlpha) {
  beautyFilter->setBlurAlpha(blurAlpha);
}

void FaceBeautyFilter::setWhite(float white) {
  beautyFilter->setWhite(white);
}

void FaceBeautyFilter::setRadius(float radius) {
  boxBlurFilter->setRadius(radius);
  boxHighPassFilter->setRadius(radius);
}
NS_GPUPIXEL_END
