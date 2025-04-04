/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "filter_group.h"
#include "gaussian_blur_filter.h"
#include "gpupixel_define.h"
#include "toon_filter.h"

namespace gpupixel {
class GPUPIXEL_API SmoothToonFilter : public FilterGroup {
 public:
  virtual ~SmoothToonFilter();

  static std::shared_ptr<SmoothToonFilter> Create();
  bool Init();

  void setBlurRadius(int blur_radius);
  void setToonThreshold(float toon_threshold);
  void setToonQuantizationLevels(float toon_quantization_levels);

 protected:
  SmoothToonFilter();

 private:
  std::shared_ptr<GaussianBlurFilter> gaussian_blur_filter_;
  std::shared_ptr<ToonFilter> toon_filter_;

  float blur_radius_;
  float toon_threshold_;
  float toon_quantization_levels_;
};

}  // namespace gpupixel
