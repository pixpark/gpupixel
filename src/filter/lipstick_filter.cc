/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "lipstick_filter.h"
#include "face_detector.h"
#include "source_image.h"

NS_GPUPIXEL_BEGIN
std::shared_ptr<LipstickFilter> LipstickFilter::create() {
  auto ret = std::shared_ptr<LipstickFilter>(new LipstickFilter());
  if (ret && !ret->init()) {
    ret.reset();
  }
  return ret;
}

bool LipstickFilter::init() {
  auto mouth = SourceImage::create(Util::getResourcePath("res/mouth.png"));
  setImageTexture(mouth);
  setTextureBounds(FrameBounds{502.5, 710, 262.5, 167.5});
  return FaceMakeupFilter::init();
}

NS_GPUPIXEL_END
