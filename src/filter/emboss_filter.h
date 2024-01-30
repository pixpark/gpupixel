/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once
#include "convolution3x3_filter.h"
#include "gpupixel_macros.h"

NS_GPUPIXEL_BEGIN

class EmbossFilter : public Convolution3x3Filter {
 public:
  static std::shared_ptr<EmbossFilter> create();
  bool init();

  void setIntensity(float intensity);

 protected:
  EmbossFilter(){};

  float _intensity;
};

NS_GPUPIXEL_END
