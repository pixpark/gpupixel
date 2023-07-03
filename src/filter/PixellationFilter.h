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

class PixellationFilter : public Filter {
 public:
  static std::shared_ptr<PixellationFilter> create();
  bool init();
  virtual bool proceed(bool bUpdateTargets = true,
                       int64_t frameTime = 0) override;

  void setPixelSize(float pixelSize);

 protected:
  PixellationFilter(){};

  float _pixelSize;
};

NS_GPUPIXEL_END
