/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once
#include "filter.h"
#include "gpupixel_macros.h"

NS_GPUPIXEL_BEGIN

class ExposureFilter : public Filter {
 public:
  static std::shared_ptr<ExposureFilter> create();
  bool init();
  virtual bool proceed(bool bUpdateTargets = true,
                       int64_t frameTime = 0) override;

  void setExposure(float exposure);

 protected:
  ExposureFilter(){};

  float _exposure;
};

NS_GPUPIXEL_END
