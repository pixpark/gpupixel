/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "ios_blur_filter.h"

namespace gpupixel {

IOSBlurFilter::IOSBlurFilter()
    : saturation_filter_(0), blur_filter_(0), luminance_range_filter_(0) {}

IOSBlurFilter::~IOSBlurFilter() {}

std::shared_ptr<IOSBlurFilter> IOSBlurFilter::Create() {
  auto ret = std::shared_ptr<IOSBlurFilter>(new IOSBlurFilter());
  if (ret && !ret->Init()) {
    ret.reset();
  }
  return ret;
}

bool IOSBlurFilter::Init() {
  if (!FilterGroup::Init()) {
    return false;
  }

  saturation_filter_ = SaturationFilter::Create();
  blur_filter_ = GaussianBlurFilter::Create();
  luminance_range_filter_ = LuminanceRangeFilter::Create();

  saturation_filter_           // 1. downsample and desaturate
      ->AddSink(blur_filter_)  // 2. apply a strong Gaussian blur
      ->AddSink(
          luminance_range_filter_);  // 3. upsample and adjust luminance range

  AddFilter(saturation_filter_);

  blur_sigma_ = 12.0;
  setBlurSigma(blur_sigma_);
  RegisterProperty("blurSigma", blur_sigma_, "",
                   [this](float& blurSigma) { setBlurSigma(blurSigma); });

  saturation_ = 0.8;
  setSaturation(saturation_);
  RegisterProperty("saturation", saturation_, "",
                   [this](float& saturation) { setSaturation(saturation); });

  range_reduction_factor_ = 0.6;
  setRangeReductionFactor(range_reduction_factor_);
  RegisterProperty("rangeReductionFactor", range_reduction_factor_, "",
                   [this](float& rangeReductionFactor) {
                     setRangeReductionFactor(rangeReductionFactor);
                   });

  down_sampling_ = 4.0;
  setDownSampling(down_sampling_);
  RegisterProperty(
      "downSampling", down_sampling_, "",
      [this](float& downSampling) { setDownSampling(downSampling); });

  return true;
}

void IOSBlurFilter::setBlurSigma(float blurSigma) {
  blur_sigma_ = blurSigma;
  blur_filter_->setSigma(blurSigma);
}

void IOSBlurFilter::setSaturation(float saturation) {
  saturation_ = saturation;
  saturation_filter_->setSaturation(saturation);
}

void IOSBlurFilter::setRangeReductionFactor(float rangeReductionFactor) {
  range_reduction_factor_ = rangeReductionFactor;
  luminance_range_filter_->setRangeReductionFactor(rangeReductionFactor);
}

void IOSBlurFilter::setDownSampling(float downSampling) {
  down_sampling_ = downSampling;
  saturation_filter_->SetFramebufferScale(1 / downSampling);
  luminance_range_filter_->SetFramebufferScale(downSampling);
}

}  // namespace gpupixel
