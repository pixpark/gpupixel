/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once
#include "filter.h"
#include "gpupixel_macros.h"
#include "math_toolbox.h"

namespace gpupixel {
class GPUPIXEL_API ColorMatrixFilter : public Filter {
 public:
  static std::shared_ptr<ColorMatrixFilter> Create();
  bool Init();

  virtual bool DoRender(bool updateSinks = true) override;

  void setIntensity(float intensity) { _intensity = intensity; }
  void setColorMatrix(Matrix4 colorMatrix) { _colorMatrix = colorMatrix; }

 protected:
  ColorMatrixFilter();

  float _intensity;
  Matrix4 _colorMatrix;
};

}
