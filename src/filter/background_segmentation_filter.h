/*
 * GPUPixel
 *
 * Created by pixpark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "filter.h"
#include "gpupixel_macros.h"

namespace gpupixel {
class GPUPIXEL_API BackgroundSegmentationFilter : public Filter {
 public:
  static std::shared_ptr<BackgroundSegmentationFilter> create();
  bool init();
  virtual bool doRender(bool updateSinks = true) override;

  // void setBrightness(float brightness);

 protected:
  BackgroundSegmentationFilter(){};

  float _brightness;
};

}
