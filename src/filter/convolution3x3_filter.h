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

namespace gpupixel {
class GPUPIXEL_API Convolution3x3Filter : public NearbySampling3x3Filter {
 public:
  virtual bool init();
  virtual bool doRender(bool updateSinks = true) override;

 protected:
  Convolution3x3Filter(){};

  // The convolution kernel is a 3x3 matrix of values to apply to the pixel and
  // its 8 surrounding pixels.
  Matrix3 _convolutionKernel;
};

}
