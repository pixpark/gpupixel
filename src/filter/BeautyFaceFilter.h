/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "FilterGroup.h"
#include "GPUPixelDef.h"

#include "BeautyFaceUnitFilter.h"
#include "BoxBlurFilter.h"
#include "BoxHighPassFilter.h"
#include "GaussianBlurFilter.h"
NS_GPUPIXEL_BEGIN

class BeautyFaceFilter : public FilterGroup {
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

 protected:
  BeautyFaceFilter();

  std::shared_ptr<BoxBlurFilter> boxBlurFilter;
  std::shared_ptr<BoxHighPassFilter> boxHighPassFilter;
  std::shared_ptr<BeautyFaceUnitFilter> beautyFilter;
};

NS_GPUPIXEL_END
