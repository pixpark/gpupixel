/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "gaussian_blur_filter.h"
#include <cmath>
#include "util.h"

namespace gpupixel {

GaussianBlurFilter::GaussianBlurFilter() : _hBlurFilter(0), _vBlurFilter(0) {}

GaussianBlurFilter::~GaussianBlurFilter() {}

std::shared_ptr<GaussianBlurFilter> GaussianBlurFilter::Create(
    int radius /* = 4*/,
    float sigma /* = 2.0*/) {
  auto ret = std::shared_ptr<GaussianBlurFilter>(new GaussianBlurFilter());
  if (ret && !ret->Init(radius, sigma)) {
    ret.reset();
  }
  return ret;
}

bool GaussianBlurFilter::Init(int radius, float sigma) {
  if (!FilterGroup::Init()) {
    return false;
  }

  _hBlurFilter = GaussianBlurMonoFilter::Create(
      GaussianBlurMonoFilter::HORIZONTAL, radius, sigma);
  _vBlurFilter = GaussianBlurMonoFilter::Create(
      GaussianBlurMonoFilter::VERTICAL, radius, sigma);
  _hBlurFilter->AddSink(_vBlurFilter);
  addFilter(_hBlurFilter);

  RegisterProperty("radius", 4, "", [this](int& radius) { SetRadius(radius); });

  RegisterProperty("sigma", 2.0, "", [this](float& sigma) { setSigma(sigma); });

  return true;
}

void GaussianBlurFilter::SetRadius(int radius) {
  _hBlurFilter->SetRadius(radius);
  _vBlurFilter->SetRadius(radius);
}

void GaussianBlurFilter::setSigma(float sigma) {
  _hBlurFilter->setSigma(sigma);
  _vBlurFilter->setSigma(sigma);
}

}
