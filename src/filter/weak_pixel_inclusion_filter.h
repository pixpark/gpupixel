/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "gpupixel_define.h"
#include "nearby_sampling3x3_filter.h"

namespace gpupixel {
class GPUPIXEL_API WeakPixelInclusionFilter : public NearbySampling3x3Filter {
 public:
  static std::shared_ptr<WeakPixelInclusionFilter> Create();
  bool Init();

 protected:
  WeakPixelInclusionFilter(){};
};

}
