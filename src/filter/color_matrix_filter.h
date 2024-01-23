/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once
#include "filter.h"
#include "gpupixel_macros.h"
#include "math_toolbox.h"

NS_GPUPIXEL_BEGIN

class ColorMatrixFilter : public Filter {
 public:
  static std::shared_ptr<ColorMatrixFilter> create();
  bool init();

  virtual bool proceed(bool bUpdateTargets = true,
                       int64_t frameTime = 0) override;

  void setIntensity(float intensity) { _intensity = intensity; }
  void setColorMatrix(Matrix4 colorMatrix) { _colorMatrix = colorMatrix; }

 protected:
  ColorMatrixFilter();

  float _intensity;
  Matrix4 _colorMatrix;
};

NS_GPUPIXEL_END
