/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "gpupixel/filter/nearby_sampling3x3_filter.h"
#include "gpupixel/gpupixel_define.h"

namespace gpupixel {
class GPUPIXEL_API DirectionalSobelEdgeDetectionFilter
    : public NearbySampling3x3Filter {
 public:
  static std::shared_ptr<DirectionalSobelEdgeDetectionFilter> Create();
  bool Init();

 protected:
  DirectionalSobelEdgeDetectionFilter() {};
};

}  // namespace gpupixel
