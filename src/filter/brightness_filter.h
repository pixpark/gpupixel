/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "filter.h"
#include "gpupixel_macros.h"

namespace gpupixel {
class GPUPIXEL_API BrightnessFilter : public Filter {
 public:
  static std::shared_ptr<BrightnessFilter> create(float brightness = 0.0);
  bool init(float brightness);
  virtual bool doRender(bool updateSinks = true) override;

  void setBrightness(float brightness);

 protected:
  BrightnessFilter(){};

  float _brightness;
};

}
