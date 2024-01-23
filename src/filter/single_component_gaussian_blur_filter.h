/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "filter_group.h"
#include "gpupixel_macros.h"
#include "single_component_gaussian_blur_mono_filter.h"

NS_GPUPIXEL_BEGIN

class SingleComponentGaussianBlurFilter : public FilterGroup {
 public:
  virtual ~SingleComponentGaussianBlurFilter();

  static std::shared_ptr<SingleComponentGaussianBlurFilter> create(
      int radius = 4,
      float sigma = 2.0);
  bool init(int radius, float sigma);
  void setRadius(int radius);
  void setSigma(float sigma);

 protected:
  SingleComponentGaussianBlurFilter();

 private:
  std::shared_ptr<SingleComponentGaussianBlurMonoFilter> _hBlurFilter;
  std::shared_ptr<SingleComponentGaussianBlurMonoFilter> _vBlurFilter;
};

NS_GPUPIXEL_END
