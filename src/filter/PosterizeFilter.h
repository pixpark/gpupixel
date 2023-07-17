/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "Filter.h"
#include "GPUPixelDef.h"

NS_GPUPIXEL_BEGIN

class PosterizeFilter : public Filter {
 public:
  static std::shared_ptr<PosterizeFilter> create();
  bool init();
  virtual bool proceed(bool bUpdateTargets = true,
                       int64_t frameTime = 0) override;

  void setColorLevels(int colorLevels);

 protected:
  PosterizeFilter(){};

  int _colorLevels;
};

NS_GPUPIXEL_END
