/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "gpupixel_macros.h"

#include "box_blur_filter.h"
#include "box_difference_filter.h"
#include "filter_group.h"
NS_GPUPIXEL_BEGIN

class BoxHighPassFilter : public FilterGroup {
 public:
  static std::shared_ptr<BoxHighPassFilter> create();
  ~BoxHighPassFilter();
  bool init();

  void setRadius(float radius);
  void setDelta(float delta);

  virtual void setInputFramebuffer(std::shared_ptr<Framebuffer> framebuffer,
                                   RotationMode rotationMode /* = NoRotation*/,
                                   int texIdx /* = 0*/) override;

 protected:
  BoxHighPassFilter();

  std::shared_ptr<BoxBlurFilter> boxBlurFilter;
  std::shared_ptr<BoxDifferenceFilter> boxDifferenceFilter;
};

NS_GPUPIXEL_END
