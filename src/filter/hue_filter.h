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
class GPUPIXEL_API HueFilter : public Filter {
 public:
  static std::shared_ptr<HueFilter> Create();
  bool Init();
  virtual bool DoRender(bool updateSinks = true) override;

  void setHueAdjustment(float hueAdjustment);

 protected:
  HueFilter(){};

  float _hueAdjustment;
};

}
