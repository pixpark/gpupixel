/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "gpupixel_macros.h"

#include "box_mono_blur_filter.h"
#include "filter_group.h"
NS_GPUPIXEL_BEGIN

class BoxBlurFilter : public FilterGroup {
 public:
  virtual ~BoxBlurFilter();

  static std::shared_ptr<BoxBlurFilter> create(int radius = 4,
                                               float sigma = 2.0);
  bool init(int radius, float sigma);
  void setRadius(int radius);
  void setSigma(float sigma);
  void setTexelSpacingMultiplier(float value);

 protected:
  BoxBlurFilter();

 private:
  std::shared_ptr<BoxMonoBlurFilter> _hBlurFilter;
  std::shared_ptr<BoxMonoBlurFilter> _vBlurFilter;
};

NS_GPUPIXEL_END
