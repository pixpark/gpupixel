/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "gpupixel_define.h"
#include "nearby_sampling3x3_filter.h"

namespace gpupixel {
class GPUPIXEL_API ToonFilter : public NearbySampling3x3Filter {
 public:
  static std::shared_ptr<ToonFilter> Create();
  bool Init();
  virtual bool DoRender(bool updateSinks = true) override;

  void setThreshold(float threshold);
  void setQuantizatinLevels(float quantizationLevels);

 protected:
  ToonFilter(){};

  float _threshold;
  float _quantizationLevels;
};

}
