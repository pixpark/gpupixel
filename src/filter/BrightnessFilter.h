/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "Filter.h"
#include "GPUPixelDef.h"

NS_GPUPIXEL_BEGIN

class BrightnessFilter : public Filter {
 public:
  static std::shared_ptr<BrightnessFilter> create(float brightness = 0.0);
  bool init(float brightness);
  virtual bool proceed(bool bUpdateTargets = true,
                       int64_t frameTime = 0) override;

  void setBrightness(float brightness);

 protected:
  BrightnessFilter(){};

  float _brightness;
};

NS_GPUPIXEL_END
