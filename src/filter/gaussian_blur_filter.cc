/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "gaussian_blur_filter.h"
#include <cmath>
#include "util.h"

NS_GPUPIXEL_BEGIN

GaussianBlurFilter::GaussianBlurFilter() : _hBlurFilter(0), _vBlurFilter(0) {}

GaussianBlurFilter::~GaussianBlurFilter() {}

std::shared_ptr<GaussianBlurFilter> GaussianBlurFilter::create(
    int radius /* = 4*/,
    float sigma /* = 2.0*/) {
  auto ret = std::shared_ptr<GaussianBlurFilter>(new GaussianBlurFilter());
  if (ret && !ret->init(radius, sigma)) {
    ret.reset();
  }
  return ret;
}

bool GaussianBlurFilter::init(int radius, float sigma) {
  if (!FilterGroup::init()) {
    return false;
  }

  _hBlurFilter = GaussianBlurMonoFilter::create(
      GaussianBlurMonoFilter::HORIZONTAL, radius, sigma);
  _vBlurFilter = GaussianBlurMonoFilter::create(
      GaussianBlurMonoFilter::VERTICAL, radius, sigma);
  _hBlurFilter->addTarget(_vBlurFilter);
  addFilter(_hBlurFilter);

  registerProperty("radius", 4, "", [this](int& radius) { setRadius(radius); });

  registerProperty("sigma", 2.0, "", [this](float& sigma) { setSigma(sigma); });

  return true;
}

void GaussianBlurFilter::setRadius(int radius) {
  _hBlurFilter->setRadius(radius);
  _vBlurFilter->setRadius(radius);
}

void GaussianBlurFilter::setSigma(float sigma) {
  _hBlurFilter->setSigma(sigma);
  _vBlurFilter->setSigma(sigma);
}

NS_GPUPIXEL_END
