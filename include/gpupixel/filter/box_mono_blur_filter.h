/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "gpupixel/gpupixel_define.h"

#include "gpupixel/filter/gaussian_blur_mono_filter.h"

namespace gpupixel {
class GPUPIXEL_API BoxMonoBlurFilter : public GaussianBlurMonoFilter {
 public:
  static std::shared_ptr<BoxMonoBlurFilter> Create(Type type = HORIZONTAL,
                                                   int radius = 4,
                                                   float sigma = 2.0);
  ~BoxMonoBlurFilter();
  bool Init(int radius, float sigma);
  void SetRadius(int radius);

 protected:
  BoxMonoBlurFilter(Type type);

  std::string GenerateOptimizedVertexShaderString(int radius,
                                                  float sigma) override;
  std::string GenerateOptimizedFragmentShaderString(int radius,
                                                    float sigma) override;
};

}  // namespace gpupixel
