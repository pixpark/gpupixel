/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "gpupixel_define.h"

#include "box_mono_blur_filter.h"
#include "filter_group.h"
namespace gpupixel {
class GPUPIXEL_API BoxBlurFilter : public FilterGroup {
 public:
  virtual ~BoxBlurFilter();

  static std::shared_ptr<BoxBlurFilter> Create(int radius = 4,
                                               float sigma = 2.0);
  bool Init(int radius, float sigma);
  void SetRadius(int radius);
  void setSigma(float sigma);
  void SetTexelSpacingMultiplier(float value);

 protected:
  BoxBlurFilter();

 private:
  std::shared_ptr<BoxMonoBlurFilter> horizontal_blur_filter_;
  std::shared_ptr<BoxMonoBlurFilter> vertical_blur_filter_;
};

}  // namespace gpupixel
