/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "gpupixel_macros.h"
#include "nearby_sampling3x3_filter.h"

NS_GPUPIXEL_BEGIN

class ToonFilter : public NearbySampling3x3Filter {
 public:
  static std::shared_ptr<ToonFilter> create();
  bool init();
  virtual bool proceed(bool bUpdateTargets = true,
                       int64_t frameTime = 0) override;

  void setThreshold(float threshold);
  void setQuantizatinLevels(float quantizationLevels);

 protected:
  ToonFilter(){};

  float _threshold;
  float _quantizationLevels;
};

NS_GPUPIXEL_END
