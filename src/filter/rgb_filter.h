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
class GPUPIXEL_API RGBFilter : public Filter {
 public:
  static std::shared_ptr<RGBFilter> Create();
  bool Init();
  virtual bool DoRender(bool updateSinks = true) override;

  void setRedAdjustment(float red_adjustment);
  void setGreenAdjustment(float green_adjustment);
  void setBlueAdjustment(float blue_adjustment);

 protected:
  RGBFilter() {};

  float red_adjustment_;
  float green_adjustment_;
  float blue_adjustment_;
};

}  // namespace gpupixel
