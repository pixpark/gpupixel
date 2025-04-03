/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "gpupixel_define.h"
#include "gaussian_blur_mono_filter.h"

namespace gpupixel {
class GPUPIXEL_API SingleComponentGaussianBlurMonoFilter
    : public GaussianBlurMonoFilter {
 public:
  static std::shared_ptr<SingleComponentGaussianBlurMonoFilter>
  Create(Type type = HORIZONTAL, int radius = 4, float sigma = 2.0);

 protected:
  SingleComponentGaussianBlurMonoFilter(Type type = HORIZONTAL);

 private:
  std::string _generateOptimizedVertexShaderString(int radius,
                                                   float sigma) override;
  std::string _generateOptimizedFragmentShaderString(int radius,
                                                     float sigma) override;
};

}
