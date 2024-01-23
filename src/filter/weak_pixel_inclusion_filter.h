/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "gpupixel_macros.h"
#include "nearby_sampling3x3_filter.h"

NS_GPUPIXEL_BEGIN

class WeakPixelInclusionFilter : public NearbySampling3x3Filter {
 public:
  static std::shared_ptr<WeakPixelInclusionFilter> create();
  bool init();

 protected:
  WeakPixelInclusionFilter(){};
};

NS_GPUPIXEL_END
