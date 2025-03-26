/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "filter.h"
#include "gpupixel_macros.h"

namespace gpupixel {
GPUPIXEL_API extern const std::string
kNearbySampling3x3SamplingVertexShaderString;

class GPUPIXEL_API NearbySampling3x3Filter : public Filter {
 public:
  virtual bool initWithFragmentShaderString(
      const std::string& fragmentShaderSource,
      int inputNumber = 1) override;
  virtual bool doRender(bool updateSinks = true) override;

  void setTexelSizeMultiplier(float texelSizeMultiplier);

 protected:
  NearbySampling3x3Filter(){};

  float _texelSizeMultiplier;
  GLuint _texelWidthUniform;
  GLuint _texelHeightUniform;
};

}
