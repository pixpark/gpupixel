/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "filter_group.h"
#include "gpupixel_macros.h"

#include "beauty_face_unit_filter.h"
#include "box_blur_filter.h"
#include "box_high_pass_filter.h"
#include "gaussian_blur_filter.h"
NS_GPUPIXEL_BEGIN
class GPUPIXEL_API BeautyFaceFilter : public FilterGroup {
 public:
  static std::shared_ptr<BeautyFaceFilter> create();
  ~BeautyFaceFilter();
  bool init();

  void setHighPassDelta(float highPassDelta);
  void setSharpen(float sharpen);
  void setBlurAlpha(float blurAlpha);
  void setWhite(float white);
  void setRadius(float sigma);

  virtual void setInputFramebuffer(std::shared_ptr<Framebuffer> framebuffer,
                                   RotationMode rotationMode /* = NoRotation*/,
                                   int texIdx /* = 0*/) override;

 BeautyFaceFilter();
 protected:

  std::shared_ptr<BoxBlurFilter> boxBlurFilter;
  std::shared_ptr<BoxHighPassFilter> boxHighPassFilter;
  std::shared_ptr<BeautyFaceUnitFilter> beautyFilter;
};

NS_GPUPIXEL_END
