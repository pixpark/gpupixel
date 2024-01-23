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

class CrosshatchFilter : public Filter {
 public:
  static std::shared_ptr<CrosshatchFilter> create();
  bool init();
  virtual bool proceed(bool bUpdateTargets = true,
                       int64_t frameTime = 0) override;

  void setCrossHatchSpacing(float crossHatchSpacing);
  void setLineWidth(float lineWidth);

 protected:
  CrosshatchFilter(){};

  float _crossHatchSpacing;
  float _lineWidth;
};

NS_GPUPIXEL_END
