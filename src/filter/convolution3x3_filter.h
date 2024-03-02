/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once
#include "gpupixel_macros.h"
#include "math_toolbox.h"
#include "nearby_sampling3x3_filter.h"

NS_GPUPIXEL_BEGIN

class Convolution3x3Filter : public NearbySampling3x3Filter {
 public:
  virtual bool init();
  virtual bool proceed(bool bUpdateTargets = true,
                       int64_t frameTime = 0) override;

 protected:
  Convolution3x3Filter(){};

  // The convolution kernel is a 3x3 matrix of values to apply to the pixel and
  // its 8 surrounding pixels.
  Matrix3 _convolutionKernel;
};

NS_GPUPIXEL_END
