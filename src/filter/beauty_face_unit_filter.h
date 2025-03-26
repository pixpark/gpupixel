/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "filter.h"
#include "gpupixel_macros.h"

namespace gpupixel {
class SourceImage;

class GPUPIXEL_API BeautyFaceUnitFilter : public Filter {
 public:
  static std::shared_ptr<BeautyFaceUnitFilter> create();
  ~BeautyFaceUnitFilter();
  bool init();
  bool doRender(bool updateSinks = true) override;

  void setSharpen(float sharpen);
  void setBlurAlpha(float blurAlpha);
  void setWhite(float white);

 protected:
  BeautyFaceUnitFilter();

  std::shared_ptr<SourceImage> grayImage_;
  std::shared_ptr<SourceImage> originImage_;
  std::shared_ptr<SourceImage> skinImage_;
  std::shared_ptr<SourceImage> customImage_;

 private:
  float sharpen_ = 0.0;
  float blurAlpha_ = 0.0;
  float white_ = 0.0;
};

}
