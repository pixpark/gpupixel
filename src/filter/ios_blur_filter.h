/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "filter_group.h"
#include "gpupixel_define.h"
#include "gaussian_blur_filter.h"
#include "luminance_range_filter.h"
#include "saturation_filter.h"

namespace gpupixel {
class GPUPIXEL_API IOSBlurFilter : public FilterGroup {
 public:
  virtual ~IOSBlurFilter();

  static std::shared_ptr<IOSBlurFilter> Create();
  bool Init();

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

}
