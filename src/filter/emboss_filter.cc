/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "emboss_filter.h"
#include "gpupixel_context.h"
namespace gpupixel {

//

std::shared_ptr<EmbossFilter> EmbossFilter::Create() {
  auto ret = std::shared_ptr<EmbossFilter>(new EmbossFilter());
  gpupixel::GPUPixelContext::GetInstance()->SyncRunWithContext([&] {
    if (ret && !ret->Init()) {
      ret.reset();
    }
  });
  return ret;
}

bool EmbossFilter::Init() {
  if (!Convolution3x3Filter::Init()) {
    return false;
  }

  intensity_factor_ = 1.0;
  setIntensity(intensity_factor_);

  RegisterProperty("intensity", intensity_factor_,
                   "The strength of the embossing, from  0.0 to 4.0, with 1.0 "
                   "as the normal level",
                   [this](float& intensity) { setIntensity(intensity); });

  return true;
}

void EmbossFilter::setIntensity(float intensity) {
  intensity_factor_ = intensity;
  if (intensity_factor_ > 4.0) {
    intensity_factor_ = 4.0;
  } else if (intensity_factor_ < 0.0) {
    intensity_factor_ = 0.0;
  }

  convolution_kernel_.set(-2.0 * intensity_factor_, -intensity_factor_, 0.0,
                          -intensity_factor_, 1.0, intensity_factor_, 0.0,
                          intensity_factor_, intensity_factor_ * 2.0);
}

}  // namespace gpupixel
