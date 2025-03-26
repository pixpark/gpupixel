/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "filter.h"
#include "gpupixel_macros.h"

namespace gpupixel {
class GPUPIXEL_API CrosshatchFilter : public Filter {
 public:
  static std::shared_ptr<CrosshatchFilter> create();
  bool init();
  virtual bool doRender(bool updateSinks = true) override;

  void setCrossHatchSpacing(float crossHatchSpacing);
  void setLineWidth(float lineWidth);

 protected:
  CrosshatchFilter(){};

  float _crossHatchSpacing;
  float _lineWidth;
};

}
