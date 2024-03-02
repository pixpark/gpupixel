/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "filter.h"
#include "gpupixel_macros.h"

NS_GPUPIXEL_BEGIN

class BoxDifferenceFilter : public Filter {
 public:
  static std::shared_ptr<BoxDifferenceFilter> create();
  ~BoxDifferenceFilter();
  bool init();
  bool proceed(bool bUpdateTargets = true, int64_t frameTime = 0) override;

  //
  void setDelta(float delta);

 protected:
  BoxDifferenceFilter();
  float delta_;
  GLuint filterTexCoordAttribute_;
  GLuint filterTexCoordAttribute2_;
};

NS_GPUPIXEL_END
