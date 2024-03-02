/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "emboss_filter.h"

USING_NS_GPUPIXEL

// REGISTER_FILTER_CLASS(EmbossFilter)

std::shared_ptr<EmbossFilter> EmbossFilter::create() {
  auto ret = std::shared_ptr<EmbossFilter>(new EmbossFilter());
  if (ret && !ret->init()) {
    ret.reset();
  }
  return ret;
}

bool EmbossFilter::init() {
  if (!Convolution3x3Filter::init()) {
    return false;
  }

  _intensity = 1.0;
  setIntensity(_intensity);

  registerProperty("intensity", _intensity,
                   "The strength of the embossing, from  0.0 to 4.0, with 1.0 "
                   "as the normal level",
                   [this](float& intensity) { setIntensity(intensity); });

  return true;
}

void EmbossFilter::setIntensity(float intensity) {
  _intensity = intensity;
  if (_intensity > 4.0) {
    _intensity = 4.0;
  } else if (_intensity < 0.0) {
    _intensity = 0.0;
  }

  _convolutionKernel.set(-2.0 * _intensity, -_intensity, 0.0, -_intensity, 1.0,
                         _intensity, 0.0, _intensity, _intensity * 2.0);
}
