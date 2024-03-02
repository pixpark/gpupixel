/*
 * GPUPixel
 *
 * Created by pixpark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "filter.h"
#include "gpupixel_macros.h"

NS_GPUPIXEL_BEGIN

class BackgroundSegmentationFilter : public Filter {
 public:
  static std::shared_ptr<BackgroundSegmentationFilter> create();
  bool init();
  virtual bool proceed(bool bUpdateTargets = true,
                       int64_t frameTime = 0) override;

  // void setBrightness(float brightness);

 protected:
  BackgroundSegmentationFilter(){};

  float _brightness;
};

NS_GPUPIXEL_END
