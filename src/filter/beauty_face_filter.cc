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

std::shared_ptr<BeautyFaceFilter> BeautyFaceFilter::Create() {
  auto ret = std::shared_ptr<BeautyFaceFilter>(new BeautyFaceFilter());
  if (ret && !ret->Init()) {
    ret.reset();
  }
  return ret;
}

bool BeautyFaceFilter::Init() {
  if (!FilterGroup::Init()) {
    return false;
  }

  boxBlurFilter = BoxBlurFilter::Create();
  addFilter(boxBlurFilter);

  boxHighPassFilter = BoxHighPassFilter::Create();
  addFilter(boxHighPassFilter);

  beautyFilter = BeautyFaceUnitFilter::Create();
  addFilter(beautyFilter);

  boxBlurFilter->AddSink(beautyFilter, 1);
  boxHighPassFilter->AddSink(beautyFilter, 2);

  setTerminalFilter(beautyFilter);

  boxBlurFilter->setTexelSpacingMultiplier(4);
  SetRadius(4);

  RegisterProperty("whiteness", 0, "The whiteness of filter with range between -1 and 1.", [this](float& val) {
      SetWhite(val);
  });

  RegisterProperty("skin_smoothing", 0, "The smoothing of filter with range between -1 and 1.", [this](float& val) {
      SetBlurAlpha(val);
  });
  return true;
}

void BeautyFaceFilter::SetInputFramebuffer(
    std::shared_ptr<GPUPixelFramebuffer> framebuffer,
    RotationMode rotationMode /* = NoRotation*/,
    int texIdx /* = 0*/) {
  for (auto& filter : _filters) {
    filter->SetInputFramebuffer(framebuffer, rotationMode, texIdx);
  }
}

void BeautyFaceFilter::SetHighPassDelta(float highPassDelta) {
  boxHighPassFilter->setDelta(highPassDelta);
}

void BeautyFaceFilter::SetSharpen(float sharpen) {
  beautyFilter->SetSharpen(sharpen);
}

void BeautyFaceFilter::SetBlurAlpha(float blurAlpha) {
  beautyFilter->SetBlurAlpha(blurAlpha);
}

void BeautyFaceFilter::SetWhite(float white) {
  beautyFilter->SetWhite(white);
}

void BeautyFaceFilter::SetRadius(float radius) {
  boxBlurFilter->SetRadius(radius);
  boxHighPassFilter->SetRadius(radius);
}
}
