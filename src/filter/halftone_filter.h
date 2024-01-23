/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "gpupixel_macros.h"
#include "pixellation_filter.h"

NS_GPUPIXEL_BEGIN

class HalftoneFilter : public PixellationFilter {
 public:
  static std::shared_ptr<HalftoneFilter> create();
  bool init();

 protected:
  HalftoneFilter(){};
};

NS_GPUPIXEL_END
