/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "filter.h"
#include "gpupixel_define.h"

namespace gpupixel {
class GPUPIXEL_API LuminanceRangeFilter : public Filter {
 public:
  static std::shared_ptr<LuminanceRangeFilter> Create();
  bool Init();
  virtual bool DoRender(bool updateSinks = true) override;

  void setRangeReductionFactor(float range_reduction_factor);

 protected:
  LuminanceRangeFilter() {};
  float range_reduction_factor_;
};

}  // namespace gpupixel
