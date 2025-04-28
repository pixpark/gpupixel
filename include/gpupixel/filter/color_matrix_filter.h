/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once
#include "gpupixel/filter/filter.h"
#include "gpupixel/gpupixel_define.h"
#include "gpupixel/utils/math_toolbox.h"

namespace gpupixel {
class GPUPIXEL_API ColorMatrixFilter : public Filter {
 public:
  static std::shared_ptr<ColorMatrixFilter> Create();
  bool Init();

  virtual bool DoRender(bool updateSinks = true) override;

  void setIntensity(float intensity) { intensity_factor_ = intensity; }
  void setColorMatrix(Matrix4 color_matrix) { color_matrix_ = color_matrix; }

 protected:
  ColorMatrixFilter();

  float intensity_factor_;
  Matrix4 color_matrix_;
};

}  // namespace gpupixel
