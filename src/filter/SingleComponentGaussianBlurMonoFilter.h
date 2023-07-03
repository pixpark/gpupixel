/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "GPUPixelDef.h"
#include "GaussianBlurMonoFilter.h"

NS_GPUPIXEL_BEGIN

class SingleComponentGaussianBlurMonoFilter : public GaussianBlurMonoFilter {
 public:
  static std::shared_ptr<SingleComponentGaussianBlurMonoFilter>
  create(Type type = HORIZONTAL, int radius = 4, float sigma = 2.0);

 protected:
  SingleComponentGaussianBlurMonoFilter(Type type = HORIZONTAL);

 private:
  std::string _generateOptimizedVertexShaderString(int radius,
                                                   float sigma) override;
  std::string _generateOptimizedFragmentShaderString(int radius,
                                                     float sigma) override;
};

NS_GPUPIXEL_END
