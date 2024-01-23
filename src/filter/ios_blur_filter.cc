/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "ios_blur_filter.h"

NS_GPUPIXEL_BEGIN

REGISTER_FILTER_CLASS(IOSBlurFilter)

IOSBlurFilter::IOSBlurFilter()
    : _saturationFilter(0), _blurFilter(0), _luminanceRangeFilter(0) {}

IOSBlurFilter::~IOSBlurFilter() {}

std::shared_ptr<IOSBlurFilter> IOSBlurFilter::create() {
  auto ret = std::shared_ptr<IOSBlurFilter>(new IOSBlurFilter());
  if (ret && !ret->init()) {
    ret.reset();
  }
  return ret;
}

bool IOSBlurFilter::init() {
  if (!FilterGroup::init()) {
    return false;
  }

  _saturationFilter = SaturationFilter::create();
  _blurFilter = GaussianBlurFilter::create();
  _luminanceRangeFilter = LuminanceRangeFilter::create();

  _saturationFilter             // 1. downsample and desaturate
      ->addTarget(_blurFilter)  // 2. apply a strong Gaussian blur
      ->addTarget(
          _luminanceRangeFilter);  // 3. upsample and adjust luminance range

  addFilter(_saturationFilter);

  _blurSigma = 12.0;
  setBlurSigma(_blurSigma);
  registerProperty("blurSigma", _blurSigma, "",
                   [this](float& blurSigma) { setBlurSigma(blurSigma); });

  _saturation = 0.8;
  setSaturation(_saturation);
  registerProperty("saturation", _saturation, "",
                   [this](float& saturation) { setSaturation(saturation); });

  _rangeReductionFactor = 0.6;
  setRangeReductionFactor(_rangeReductionFactor);
  registerProperty("rangeReductionFactor", _rangeReductionFactor, "",
                   [this](float& rangeReductionFactor) {
                     setRangeReductionFactor(rangeReductionFactor);
                   });

  _downSampling = 4.0;
  setDownSampling(_downSampling);
  registerProperty(
      "downSampling", _downSampling, "",
      [this](float& downSampling) { setDownSampling(downSampling); });

  return true;
}

void IOSBlurFilter::setBlurSigma(float blurSigma) {
  _blurSigma = blurSigma;
  _blurFilter->setSigma(blurSigma);
}

void IOSBlurFilter::setSaturation(float saturation) {
  _saturation = saturation;
  _saturationFilter->setSaturation(saturation);
}

void IOSBlurFilter::setRangeReductionFactor(float rangeReductionFactor) {
  _rangeReductionFactor = rangeReductionFactor;
  _luminanceRangeFilter->setRangeReductionFactor(rangeReductionFactor);
}

void IOSBlurFilter::setDownSampling(float downSampling) {
  _downSampling = downSampling;
  _saturationFilter->setFramebufferScale(1 / downSampling);
  _luminanceRangeFilter->setFramebufferScale(downSampling);
}

NS_GPUPIXEL_END
