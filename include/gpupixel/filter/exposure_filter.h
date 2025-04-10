/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once
#include "gpupixel/gpupixel_define.h"
#include "gpupixel/filter/filter.h"

namespace gpupixel {
class GPUPIXEL_API ExposureFilter : public Filter {
 public:
  static std::shared_ptr<ExposureFilter> Create();
  bool Init();
  virtual bool DoRender(bool updateSinks = true) override;

  void SetExposure(float exposure);

 protected:
  ExposureFilter() {};

  float exposure_factor_;
};

}  // namespace gpupixel
