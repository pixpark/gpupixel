/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "gpupixel/filter/filter.h"
#include "gpupixel/gpupixel_define.h"

namespace gpupixel {
class GPUPIXEL_API BoxDifferenceFilter : public Filter {
 public:
  static std::shared_ptr<BoxDifferenceFilter> Create();
  ~BoxDifferenceFilter();
  bool Init();
  bool DoRender(bool updateSinks = true) override;

  //
  void SetDelta(float delta);

 protected:
  BoxDifferenceFilter();
  float delta_;
  uint32_t filter_texture_coordinate_attribute_;
  uint32_t filter_texture_coordinate_attribute2_;
};

}  // namespace gpupixel
