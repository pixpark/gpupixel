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
class GPUPIXEL_API WhiteBalanceFilter : public Filter {
 public:
  static std::shared_ptr<WhiteBalanceFilter> Create();
  bool Init();
  virtual bool DoRender(bool updateSinks = true) override;

  void setTemperature(float temperature);
  void setTint(float tint);

 protected:
  WhiteBalanceFilter() {};

  float temperature_;
  float tint_;
};

}  // namespace gpupixel
