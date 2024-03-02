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

class WhiteBalanceFilter : public Filter {
 public:
  static std::shared_ptr<WhiteBalanceFilter> create();
  bool init();
  virtual bool proceed(bool bUpdateTargets = true,
                       int64_t frameTime = 0) override;

  void setTemperature(float temperature);
  void setTint(float tint);

 protected:
  WhiteBalanceFilter(){};

  float _temperature;
  float _tint;
};

NS_GPUPIXEL_END
