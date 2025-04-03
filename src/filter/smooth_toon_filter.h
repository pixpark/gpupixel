/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "filter_group.h"
#include "gpupixel_macros.h"
#include "gaussian_blur_filter.h"
#include "toon_filter.h"

namespace gpupixel {
class GPUPIXEL_API SmoothToonFilter : public FilterGroup {
 public:
  virtual ~SmoothToonFilter();

  static std::shared_ptr<SmoothToonFilter> Create();
  bool Init();

  void setBlurRadius(int blurRadius);
  void setToonThreshold(float toonThreshold);
  void setToonQuantizationLevels(float toonQuantizationLevels);

 protected:
  SmoothToonFilter();

 private:
  std::shared_ptr<GaussianBlurFilter> _gaussianBlurFilter;
  std::shared_ptr<ToonFilter> _toonFilter;

  float _blurRadius;
  float _toonThreshold;
  float _toonQuantizationLevels;
};

}
