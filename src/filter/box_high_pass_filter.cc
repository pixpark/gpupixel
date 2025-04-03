/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "box_high_pass_filter.h"

namespace gpupixel {

BoxHighPassFilter::BoxHighPassFilter() {}

BoxHighPassFilter::~BoxHighPassFilter() {}

std::shared_ptr<BoxHighPassFilter> BoxHighPassFilter::Create() {
  auto ret = std::shared_ptr<BoxHighPassFilter>(new BoxHighPassFilter());
  if (ret && !ret->Init()) {
    ret.reset();
  }
  return ret;
}

bool BoxHighPassFilter::Init() {
  if (!FilterGroup::Init()) {
    return false;
  }

  boxBlurFilter = BoxBlurFilter::Create();
  addFilter(boxBlurFilter);

  boxDifferenceFilter = BoxDifferenceFilter::Create();
  addFilter(boxDifferenceFilter);

  boxBlurFilter->AddSink(boxDifferenceFilter, 1);
  setTerminalFilter(boxDifferenceFilter);

  boxBlurFilter->setTexelSpacingMultiplier(4);
  return true;
}

void BoxHighPassFilter::SetInputFramebuffer(
    std::shared_ptr<GPUPixelFramebuffer> framebuffer,
    RotationMode rotationMode /* = NoRotation*/,
    int texIdx /* = 0*/) {
  for (auto& filter : _filters) {
    filter->SetInputFramebuffer(framebuffer, rotationMode, texIdx);
  }
}

void BoxHighPassFilter::SetRadius(float radius) {
  boxBlurFilter->SetRadius(radius);
}

void BoxHighPassFilter::setDelta(float delta) {
  boxDifferenceFilter->setDelta(delta);
}

}
