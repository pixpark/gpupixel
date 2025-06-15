/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "gpupixel/filter/nearby_sampling3x3_filter.h"
#include "gpupixel/gpupixel_define.h"

namespace gpupixel {
class GPUPIXEL_API Convolution3x3Filter : public NearbySampling3x3Filter {
 public:
  virtual bool Init();
  virtual bool DoRender(bool updateSinks = true) override;

 protected:
  Convolution3x3Filter() {};

  // The convolution kernel is a 3x3 matrix of values to apply to the pixel and
  // its 8 surrounding pixels.
  glm::mat3 convolution_kernel_;
};

}  // namespace gpupixel
