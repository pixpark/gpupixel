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
class GPUPIXEL_API ColorInvertFilter : public Filter {
 public:
  static std::shared_ptr<ColorInvertFilter> Create();
  bool Init();

  virtual bool DoRender(bool updateSinks = true) override;

 protected:
  ColorInvertFilter() {};
};

}  // namespace gpupixel
