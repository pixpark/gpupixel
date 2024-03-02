/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "gpupixel_macros.h"

#include "gaussian_blur_mono_filter.h"

NS_GPUPIXEL_BEGIN

class BoxMonoBlurFilter : public GaussianBlurMonoFilter {
 public:
  static std::shared_ptr<BoxMonoBlurFilter> create(Type type = HORIZONTAL,
                                                   int radius = 4,
                                                   float sigma = 2.0);
  ~BoxMonoBlurFilter();
  bool init(int radius, float sigma);
  void setRadius(int radius);

 protected:
  BoxMonoBlurFilter(Type type);

  std::string _generateOptimizedVertexShaderString(int radius,
                                                   float sigma) override;
  std::string _generateOptimizedFragmentShaderString(int radius,
                                                     float sigma) override;
};

NS_GPUPIXEL_END
