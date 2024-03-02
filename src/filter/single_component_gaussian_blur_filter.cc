/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "single_component_gaussian_blur_filter.h"
#include <cmath>
#include "util.h"

NS_GPUPIXEL_BEGIN

SingleComponentGaussianBlurFilter::SingleComponentGaussianBlurFilter()
    : _hBlurFilter(0), _vBlurFilter(0) {}

SingleComponentGaussianBlurFilter::~SingleComponentGaussianBlurFilter() {}

std::shared_ptr<SingleComponentGaussianBlurFilter>
SingleComponentGaussianBlurFilter::create(int radius /* = 4*/,
                                          float sigma /* = 2.0*/) {
  auto ret = std::shared_ptr<SingleComponentGaussianBlurFilter>(
      new SingleComponentGaussianBlurFilter());
  if (ret && !ret->init(radius, sigma)) {
    ret.reset();
  }
  return ret;
}

bool SingleComponentGaussianBlurFilter::init(int radius, float sigma) {
  if (!FilterGroup::init()) {
    return false;
  }

  _hBlurFilter = SingleComponentGaussianBlurMonoFilter::create(
      SingleComponentGaussianBlurMonoFilter::HORIZONTAL, radius, sigma);
  _vBlurFilter = SingleComponentGaussianBlurMonoFilter::create(
      SingleComponentGaussianBlurMonoFilter::VERTICAL, radius, sigma);
  _hBlurFilter->addTarget(_vBlurFilter);
  addFilter(_hBlurFilter);

  registerProperty("radius", 4, "", [this](int& radius) { setRadius(radius); });

  registerProperty("sigma", 2.0, "", [this](float& sigma) { setSigma(sigma); });

  return true;
}

void SingleComponentGaussianBlurFilter::setRadius(int radius) {
  _hBlurFilter->setRadius(radius);
  _vBlurFilter->setRadius(radius);
}

void SingleComponentGaussianBlurFilter::setSigma(float sigma) {
  _hBlurFilter->setSigma(sigma);
  _vBlurFilter->setSigma(sigma);
}

NS_GPUPIXEL_END
