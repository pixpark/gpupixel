/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "gpupixel_macros.h"
#include "nearby_sampling3x3_filter.h"

NS_GPUPIXEL_BEGIN

class DirectionalSobelEdgeDetectionFilter : public NearbySampling3x3Filter {
 public:
  static std::shared_ptr<DirectionalSobelEdgeDetectionFilter> create();
  bool init();

 protected:
  DirectionalSobelEdgeDetectionFilter(){};
};

NS_GPUPIXEL_END
