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

  box_blur_filter_ = BoxBlurFilter::Create();
  AddFilter(box_blur_filter_);

  box_difference_filter_ = BoxDifferenceFilter::Create();
  AddFilter(box_difference_filter_);

  box_blur_filter_->AddSink(box_difference_filter_, 1);
  SetTerminalFilter(box_difference_filter_);

  box_blur_filter_->SetTexelSpacingMultiplier(4);
  return true;
}

void BoxHighPassFilter::SetInputFramebuffer(
    std::shared_ptr<GPUPixelFramebuffer> framebuffer,
    RotationMode rotation_mode /* = NoRotation*/,
    int texIdx /* = 0*/) {
  for (auto& filter : filters_) {
    filter->SetInputFramebuffer(framebuffer, rotation_mode, texIdx);
  }
}

void BoxHighPassFilter::SetRadius(float radius) {
  box_blur_filter_->SetRadius(radius);
}

void BoxHighPassFilter::SetDelta(float delta) {
  box_difference_filter_->SetDelta(delta);
}

}  // namespace gpupixel
