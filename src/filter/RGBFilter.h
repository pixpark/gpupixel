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

class RGBFilter : public Filter {
 public:
  static std::shared_ptr<RGBFilter> create();
  bool init();
  virtual bool proceed(bool bUpdateTargets = true,
                       int64_t frameTime = 0) override;

  void setRedAdjustment(float redAdjustment);
  void setGreenAdjustment(float greenAdjustment);
  void setBlueAdjustment(float blueAdjustment);

 protected:
  RGBFilter(){};

  float _redAdjustment;
  float _greenAdjustment;
  float _blueAdjustment;
};

NS_GPUPIXEL_END
