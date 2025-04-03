/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "gpupixel_macros.h"
#include "pixellation_filter.h"

namespace gpupixel {
class GPUPIXEL_API HalftoneFilter : public PixellationFilter {
 public:
  static std::shared_ptr<HalftoneFilter> Create();
  bool Init();

 protected:
  HalftoneFilter(){};
};

}
