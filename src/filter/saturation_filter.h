/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "filter.h"
#include "gpupixel_macros.h"

NS_GPUPIXEL_BEGIN

class SaturationFilter : public Filter {
 public:
  static std::shared_ptr<SaturationFilter> create();
  bool init();
  virtual bool proceed(bool bUpdateTargets = true,
                       int64_t frameTime = 0) override;

  void setSaturation(float saturation);

 protected:
  SaturationFilter(){};

  float _saturation;
};

NS_GPUPIXEL_END
