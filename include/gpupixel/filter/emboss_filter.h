/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once
#include "gpupixel/filter/convolution3x3_filter.h"
#include "gpupixel/gpupixel_define.h"

namespace gpupixel {
class GPUPIXEL_API EmbossFilter : public Convolution3x3Filter {
 public:
  static std::shared_ptr<EmbossFilter> Create();
  bool Init();

  void setIntensity(float intensity);

 protected:
  EmbossFilter() {};

  float intensity_factor_;
};

}  // namespace gpupixel
