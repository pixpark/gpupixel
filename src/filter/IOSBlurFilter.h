/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "FilterGroup.h"
#include "GPUPixelDef.h"
#include "GaussianBlurFilter.h"
#include "LuminanceRangeFilter.h"
#include "SaturationFilter.h"

NS_GPUPIXEL_BEGIN

class IOSBlurFilter : public FilterGroup {
 public:
  virtual ~IOSBlurFilter();

  static std::shared_ptr<IOSBlurFilter> create();
  bool init();

  void setBlurSigma(float blurSigma);
  void setSaturation(float saturation);
  void setRangeReductionFactor(float rangeReductionFactor);
  void setDownSampling(float downSampling);

 protected:
  IOSBlurFilter();
  std::shared_ptr<SaturationFilter> _saturationFilter;
  std::shared_ptr<GaussianBlurFilter> _blurFilter;
  std::shared_ptr<LuminanceRangeFilter> _luminanceRangeFilter;

  float _blurSigma;
  float _saturation;
  float _rangeReductionFactor;
  float _downSampling;
};

NS_GPUPIXEL_END
