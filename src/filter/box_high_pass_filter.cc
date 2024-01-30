/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "box_high_pass_filter.h"

NS_GPUPIXEL_BEGIN

BoxHighPassFilter::BoxHighPassFilter() {}

BoxHighPassFilter::~BoxHighPassFilter() {}

std::shared_ptr<BoxHighPassFilter> BoxHighPassFilter::create() {
  auto ret = std::shared_ptr<BoxHighPassFilter>(new BoxHighPassFilter());
  if (ret && !ret->init()) {
    ret.reset();
  }
  return ret;
}

bool BoxHighPassFilter::init() {
  if (!FilterGroup::init()) {
    return false;
  }

  boxBlurFilter = BoxBlurFilter::create();
  addFilter(boxBlurFilter);

  boxDifferenceFilter = BoxDifferenceFilter::create();
  addFilter(boxDifferenceFilter);

  boxBlurFilter->addTarget(boxDifferenceFilter, 1);
  setTerminalFilter(boxDifferenceFilter);

  boxBlurFilter->setTexelSpacingMultiplier(4);
  return true;
}

void BoxHighPassFilter::setInputFramebuffer(
    std::shared_ptr<Framebuffer> framebuffer,
    RotationMode rotationMode /* = NoRotation*/,
    int texIdx /* = 0*/) {
  for (auto& filter : _filters) {
    filter->setInputFramebuffer(framebuffer, rotationMode, texIdx);
  }
}

void BoxHighPassFilter::setRadius(float radius) {
  boxBlurFilter->setRadius(radius);
}

void BoxHighPassFilter::setDelta(float delta) {
  boxDifferenceFilter->setDelta(delta);
}

NS_GPUPIXEL_END
