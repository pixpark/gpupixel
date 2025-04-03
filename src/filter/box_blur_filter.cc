/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "box_blur_filter.h"

namespace gpupixel {

BoxBlurFilter::BoxBlurFilter() : _hBlurFilter(0), _vBlurFilter(0) {}

BoxBlurFilter::~BoxBlurFilter() {}

std::shared_ptr<BoxBlurFilter> BoxBlurFilter::Create(int radius /* = 4*/,
                                                     float sigma /* = 2.0*/) {
  auto ret = std::shared_ptr<BoxBlurFilter>(new BoxBlurFilter());
  if (ret && !ret->Init(radius, sigma)) {
    ret.reset();
  }
  return ret;
}

bool BoxBlurFilter::Init(int radius, float sigma) {
  if (!FilterGroup::Init()) {
    return false;
  }

  _hBlurFilter =
      BoxMonoBlurFilter::Create(GaussianBlurMonoFilter::HORIZONTAL, 4, 0.0);
  _vBlurFilter =
      BoxMonoBlurFilter::Create(GaussianBlurMonoFilter::VERTICAL, 4, 0.0);
  _hBlurFilter->AddSink(_vBlurFilter);
  addFilter(_hBlurFilter);

  RegisterProperty("radius", 4, "", [this](int& radius) { SetRadius(radius); });

  RegisterProperty("sigma", 0.0, "", [this](float& sigma) { setSigma(sigma); });

  return true;
}

void BoxBlurFilter::SetRadius(int radius) {
  _hBlurFilter->SetRadius(radius);
  _vBlurFilter->SetRadius(radius);
}

void BoxBlurFilter::setSigma(float sigma) {
  _hBlurFilter->setSigma(sigma);
  _vBlurFilter->setSigma(sigma);
}

void BoxBlurFilter::setTexelSpacingMultiplier(float value) {
  _hBlurFilter->setTexelSpacingMultiplier(value);
  _vBlurFilter->setTexelSpacingMultiplier(value);
}

}
