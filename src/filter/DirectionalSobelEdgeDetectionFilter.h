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

class DirectionalSobelEdgeDetectionFilter : public NearbySampling3x3Filter {
 public:
  static std::shared_ptr<DirectionalSobelEdgeDetectionFilter> create();
  bool init();

 protected:
  DirectionalSobelEdgeDetectionFilter(){};
};

NS_GPUPIXEL_END
