/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "gpupixel_define.h"

#include "box_blur_filter.h"
#include "box_difference_filter.h"
#include "filter_group.h"
namespace gpupixel {
class GPUPIXEL_API BoxHighPassFilter : public FilterGroup {
 public:
  static std::shared_ptr<BoxHighPassFilter> Create();
  ~BoxHighPassFilter();
  bool Init();

  void SetRadius(float radius);
  void SetDelta(float delta);

  virtual void SetInputFramebuffer(
      std::shared_ptr<GPUPixelFramebuffer> framebuffer,
      RotationMode rotation_mode /* = NoRotation*/,
      int texIdx /* = 0*/) override;

 protected:
  BoxHighPassFilter();

  std::shared_ptr<BoxBlurFilter> box_blur_filter_;
  std::shared_ptr<BoxDifferenceFilter> box_difference_filter_;
};

}  // namespace gpupixel
