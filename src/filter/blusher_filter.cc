/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "blusher_filter.h"
#include "face_detector.h"
#include "source_image.h"

NS_GPUPIXEL_BEGIN
std::shared_ptr<BlusherFilter> BlusherFilter::create() {
  auto ret = std::shared_ptr<BlusherFilter>(new BlusherFilter());
  if (ret && !ret->init()) {
    ret.reset();
  }
  return ret;
}

bool BlusherFilter::init() {
  auto blusher  = SourceImage::create(Util::getResourcePath("res/blusher.png"));
  setImageTexture(blusher);
  setTextureBounds(FrameBounds{395, 520, 489, 209});
  return FaceMakeupFilter::init();
}

NS_GPUPIXEL_END
