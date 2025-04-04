/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "filter.h"
#include "gpupixel_define.h"

namespace gpupixel {
GPUPIXEL_API extern const std::string
    kNearbySampling3x3SamplingVertexShaderString;

class GPUPIXEL_API NearbySampling3x3Filter : public Filter {
 public:
  virtual bool InitWithFragmentShaderString(
      const std::string& fragmentShaderSource,
      int inputNumber = 1) override;
  virtual bool DoRender(bool updateSinks = true) override;

  void setTexelSizeMultiplier(float texel_size_multiplier);

 protected:
  NearbySampling3x3Filter() {};

  float texel_size_multiplier_;
  GLuint texel_width_uniform_;
  GLuint texel_height_uniform_;
};

}  // namespace gpupixel
