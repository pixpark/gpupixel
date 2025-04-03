/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "emboss_filter.h"

namespace gpupixel {

// REGISTER_FILTER_CLASS(EmbossFilter)

std::shared_ptr<EmbossFilter> EmbossFilter::Create() {
  auto ret = std::shared_ptr<EmbossFilter>(new EmbossFilter());
  if (ret && !ret->Init()) {
    ret.reset();
  }
  return ret;
}

bool EmbossFilter::Init() {
  if (!Convolution3x3Filter::Init()) {
    return false;
  }

  _intensity = 1.0;
  setIntensity(_intensity);

  RegisterProperty("intensity", _intensity,
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

} // namespace gpupixel