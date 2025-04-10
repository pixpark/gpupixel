/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "gpupixel/gpupixel_define.h"
#include "gpupixel/filter/filter_group.h"
#include "gpupixel/filter/single_component_gaussian_blur_mono_filter.h"

namespace gpupixel {
class GPUPIXEL_API SingleComponentGaussianBlurFilter : public FilterGroup {
 public:
  virtual ~SingleComponentGaussianBlurFilter();

  static std::shared_ptr<SingleComponentGaussianBlurFilter> Create(
      int radius = 4,
      float sigma = 2.0);
  bool Init(int radius, float sigma);
  void SetRadius(int radius);
  void setSigma(float sigma);

 protected:
  SingleComponentGaussianBlurFilter();

 private:
  std::shared_ptr<SingleComponentGaussianBlurMonoFilter> h_blur_filter_;
  std::shared_ptr<SingleComponentGaussianBlurMonoFilter> v_blur_filter_;
};

}  // namespace gpupixel
