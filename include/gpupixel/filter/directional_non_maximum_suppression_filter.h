/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "gpupixel/filter/filter.h"
#include "gpupixel/gpupixel_define.h"

namespace gpupixel {
class GPUPIXEL_API DirectionalNonMaximumSuppressionFilter : public Filter {
 public:
  static std::shared_ptr<DirectionalNonMaximumSuppressionFilter> Create();
  bool Init();

  virtual bool DoRender(bool updateSinks = true) override;

 protected:
  uint32_t texel_width_uniform_;
  uint32_t texel_height_uniform_;
  DirectionalNonMaximumSuppressionFilter() {};
};

}  // namespace gpupixel
