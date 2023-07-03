/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "BoxBlurFilter.h"

NS_GPUPIXEL_BEGIN

BoxBlurFilter::BoxBlurFilter() : _hBlurFilter(0), _vBlurFilter(0) {}

BoxBlurFilter::~BoxBlurFilter() {}

std::shared_ptr<BoxBlurFilter> BoxBlurFilter::create(int radius /* = 4*/,
                                                     float sigma /* = 2.0*/) {
  auto ret = std::shared_ptr<BoxBlurFilter>(new BoxBlurFilter());
  if (ret && !ret->init(radius, sigma)) {
    ret.reset();
  }
  return ret;
}

bool BoxBlurFilter::init(int radius, float sigma) {
  if (!FilterGroup::init()) {
    return false;
  }

  _hBlurFilter =
      BoxMonoBlurFilter::create(GaussianBlurMonoFilter::HORIZONTAL, 4, 0.0);
  _vBlurFilter =
      BoxMonoBlurFilter::create(GaussianBlurMonoFilter::VERTICAL, 4, 0.0);
  _hBlurFilter->addTarget(_vBlurFilter);
  addFilter(_hBlurFilter);

  registerProperty("radius", 4, "", [this](int& radius) { setRadius(radius); });

  registerProperty("sigma", 0.0, "", [this](float& sigma) { setSigma(sigma); });

  return true;
}

void BoxBlurFilter::setRadius(int radius) {
  _hBlurFilter->setRadius(radius);
  _vBlurFilter->setRadius(radius);
}

void BoxBlurFilter::setSigma(float sigma) {
  _hBlurFilter->setSigma(sigma);
  _vBlurFilter->setSigma(sigma);
}

void BoxBlurFilter::setTexelSpacingMultiplier(float value) {
  _vBlurFilter->setTexelSpacingMultiplier(value);
  _vBlurFilter->setTexelSpacingMultiplier(value);
}

NS_GPUPIXEL_END
