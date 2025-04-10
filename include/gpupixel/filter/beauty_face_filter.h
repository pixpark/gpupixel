/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "gpupixel/filter/beauty_face_filter.h"
#include "gpupixel/filter/beauty_face_unit_filter.h"
#include "gpupixel/filter/box_blur_filter.h"
#include "gpupixel/filter/box_high_pass_filter.h"
#include "gpupixel/filter/gaussian_blur_filter.h"

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

  virtual void SetInputFramebuffer(
      std::shared_ptr<GPUPixelFramebuffer> framebuffer,
      RotationMode rotation_mode = NoRotation,
      int texIdx = 0) override;

 private:
  BeautyFaceFilter();
  std::shared_ptr<BoxBlurFilter> box_blur_filter_;
  std::shared_ptr<BoxHighPassFilter> box_high_pass_filter_;
  std::shared_ptr<BeautyFaceUnitFilter> beauty_face_filter_;
};

}  // namespace gpupixel
