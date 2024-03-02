/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "filter_group.h"
#include "gpupixel_macros.h"
#include "gaussian_blur_mono_filter.h"

NS_GPUPIXEL_BEGIN

class GaussianBlurFilter : public FilterGroup {
 public:
  virtual ~GaussianBlurFilter();

  static std::shared_ptr<GaussianBlurFilter> create(int radius = 4,
                                                    float sigma = 2.0);
  bool init(int radius, float sigma);
  void setRadius(int radius);
  void setSigma(float sigma);

 protected:
  GaussianBlurFilter();

 private:
  std::shared_ptr<GaussianBlurMonoFilter> _hBlurFilter;
  std::shared_ptr<GaussianBlurMonoFilter> _vBlurFilter;
};

NS_GPUPIXEL_END
