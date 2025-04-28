/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "gpupixel/filter/filter_group.h"
#include "gpupixel/filter/gaussian_blur_filter.h"
#include "gpupixel/filter/luminance_range_filter.h"
#include "gpupixel/filter/saturation_filter.h"
#include "gpupixel/gpupixel_define.h"

namespace gpupixel {
class GPUPIXEL_API IOSBlurFilter : public FilterGroup {
 public:
  virtual ~IOSBlurFilter();

  static std::shared_ptr<IOSBlurFilter> Create();
  bool Init();

  void setBlurSigma(float blur_sigma);
  void setSaturation(float saturation);
  void setRangeReductionFactor(float range_reduction_factor);
  void setDownSampling(float down_sampling);

 protected:
  IOSBlurFilter();
  std::shared_ptr<SaturationFilter> saturation_filter_;
  std::shared_ptr<GaussianBlurFilter> blur_filter_;
  std::shared_ptr<LuminanceRangeFilter> luminance_range_filter_;

  float blur_sigma_;
  float saturation_;
  float range_reduction_factor_;
  float down_sampling_;
};

}  // namespace gpupixel
