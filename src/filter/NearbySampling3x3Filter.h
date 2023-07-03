/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "Filter.h"
#include "GPUPixelDef.h"

NS_GPUPIXEL_BEGIN

extern const std::string kNearbySampling3x3SamplingVertexShaderString;

class NearbySampling3x3Filter : public Filter {
 public:
  virtual bool initWithFragmentShaderString(
      const std::string& fragmentShaderSource,
      int inputNumber = 1) override;
  virtual bool proceed(bool bUpdateTargets = true,
                       int64_t frameTime = 0) override;

  void setTexelSizeMultiplier(float texelSizeMultiplier);

 protected:
  NearbySampling3x3Filter(){};

  float _texelSizeMultiplier;
  GLuint _texelWidthUniform;
  GLuint _texelHeightUniform;
};

NS_GPUPIXEL_END
