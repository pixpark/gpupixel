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
class GPUPIXEL_API SaturationFilter : public Filter {
 public:
  static std::shared_ptr<SaturationFilter> Create();
  bool Init();
  virtual bool DoRender(bool updateSinks = true) override;

  void setSaturation(float saturation);

 protected:
  SaturationFilter() {};

  float saturation_;
};

}  // namespace gpupixel
