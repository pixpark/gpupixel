/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "filter.h"
#include "gpupixel_define.h"

namespace gpupixel {
class SourceImage;

class GPUPIXEL_API BeautyFaceUnitFilter : public Filter {
 public:
  static std::shared_ptr<BeautyFaceUnitFilter> Create();
  ~BeautyFaceUnitFilter();
  bool Init();
  bool DoRender(bool updateSinks = true) override;

  void SetSharpen(float sharpen);
  void SetBlurAlpha(float blurAlpha);
  void SetWhite(float white);

 protected:
  BeautyFaceUnitFilter();

  std::shared_ptr<SourceImage> gray_image_;
  std::shared_ptr<SourceImage> original_image_;
  std::shared_ptr<SourceImage> skin_image_;
  std::shared_ptr<SourceImage> custom_image_;

 private:
  float sharpen_factor_ = 0.0;
  float blur_alpha_ = 0.0;
  float white_balance_ = 0.0;
};

}  // namespace gpupixel
