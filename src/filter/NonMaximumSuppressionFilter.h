/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "GPUPixelDef.h"
#include "NearbySampling3x3Filter.h"

NS_GPUPIXEL_BEGIN

class NonMaximumSuppressionFilter : public NearbySampling3x3Filter {
 public:
  static std::shared_ptr<NonMaximumSuppressionFilter> create();
  bool init();

 protected:
  NonMaximumSuppressionFilter(){};
};

NS_GPUPIXEL_END
