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
class GPUPIXEL_API CrosshatchFilter : public Filter {
 public:
  static std::shared_ptr<CrosshatchFilter> Create();
  bool Init();
  virtual bool DoRender(bool updateSinks = true) override;

  void setCrossHatchSpacing(float cross_hatch_spacing);
  void setLineWidth(float line_width);

 protected:
  CrosshatchFilter() {};

  float cross_hatch_spacing_;
  float line_width_;
};

}  // namespace gpupixel
