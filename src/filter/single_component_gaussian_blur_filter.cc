/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "single_component_gaussian_blur_filter.h"
#include <cmath>
#include "util.h"

namespace gpupixel {

SingleComponentGaussianBlurFilter::SingleComponentGaussianBlurFilter()
    : _hBlurFilter(0), _vBlurFilter(0) {}

SingleComponentGaussianBlurFilter::~SingleComponentGaussianBlurFilter() {}

std::shared_ptr<SingleComponentGaussianBlurFilter>
SingleComponentGaussianBlurFilter::Create(int radius /* = 4*/,
                                          float sigma /* = 2.0*/) {
  auto ret = std::shared_ptr<SingleComponentGaussianBlurFilter>(
      new SingleComponentGaussianBlurFilter());
  if (ret && !ret->Init(radius, sigma)) {
    ret.reset();
  }
  return ret;
}

bool SingleComponentGaussianBlurFilter::Init(int radius, float sigma) {
  if (!FilterGroup::Init()) {
    return false;
  }

  _hBlurFilter = SingleComponentGaussianBlurMonoFilter::Create(
      SingleComponentGaussianBlurMonoFilter::HORIZONTAL, radius, sigma);
  _vBlurFilter = SingleComponentGaussianBlurMonoFilter::Create(
      SingleComponentGaussianBlurMonoFilter::VERTICAL, radius, sigma);
  _hBlurFilter->AddSink(_vBlurFilter);
  addFilter(_hBlurFilter);

  RegisterProperty("radius", 4, "", [this](int& radius) { SetRadius(radius); });

  RegisterProperty("sigma", 2.0, "", [this](float& sigma) { setSigma(sigma); });

  return true;
}

void SingleComponentGaussianBlurFilter::SetRadius(int radius) {
  _hBlurFilter->SetRadius(radius);
  _vBlurFilter->SetRadius(radius);
}

void SingleComponentGaussianBlurFilter::setSigma(float sigma) {
  _hBlurFilter->setSigma(sigma);
  _vBlurFilter->setSigma(sigma);
}

}
