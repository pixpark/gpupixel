/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "filter_group.h"
#include "gpupixel_define.h"

#include "beauty_face_unit_filter.h"
#include "box_blur_filter.h"
#include "box_high_pass_filter.h"
#include "gaussian_blur_filter.h"

namespace gpupixel {
class GPUPIXEL_API BeautyFaceFilter : public FilterGroup {
 public:
  static std::shared_ptr<BeautyFaceFilter> Create();
  ~BeautyFaceFilter();
  bool Init();

  void SetHighPassDelta(float highPassDelta);
  void SetSharpen(float sharpen);
  void SetBlurAlpha(float blurAlpha);
  void SetWhite(float white);
  void SetRadius(float sigma);

  virtual void SetInputFramebuffer(std::shared_ptr<GPUPixelFramebuffer> framebuffer,
                                   RotationMode rotationMode /* = NoRotation*/,
                                   int texIdx /* = 0*/) override;

 protected:
  BeautyFaceFilter();

  std::shared_ptr<BoxBlurFilter> boxBlurFilter;
  std::shared_ptr<BoxHighPassFilter> boxHighPassFilter;
  std::shared_ptr<BeautyFaceUnitFilter> beautyFilter;
};

}
