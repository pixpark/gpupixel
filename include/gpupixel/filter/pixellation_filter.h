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
class GPUPIXEL_API PixellationFilter : public Filter {
 public:
  static std::shared_ptr<PixellationFilter> Create();
  bool Init();
  virtual bool DoRender(bool updateSinks = true) override;

  void setPixelSize(float pixel_size);

 protected:
  PixellationFilter() {};

  float pixel_size_;
};

}  // namespace gpupixel
