/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "gpupixel/gpupixel_define.h"
#include "gpupixel/filter/filter.h"

namespace gpupixel {
class GPUPIXEL_API BrightnessFilter : public Filter {
 public:
  static std::shared_ptr<BrightnessFilter> Create(float brightness = 0.0);
  bool Init(float brightness);
  virtual bool DoRender(bool updateSinks = true) override;

  void setBrightness(float brightness);

 protected:
  BrightnessFilter() {};

  float brightness_factor_;
};

}  // namespace gpupixel
