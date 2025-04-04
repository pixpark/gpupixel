/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "blusher_filter.h"
#include "source_image.h"

namespace gpupixel {
std::shared_ptr<BlusherFilter> BlusherFilter::Create() {
  auto ret = std::shared_ptr<BlusherFilter>(new BlusherFilter());
  if (ret && !ret->Init()) {
    ret.reset();
  }
  return ret;
}

bool BlusherFilter::Init() {
  auto blusher = SourceImage::Create(Util::getResourcePath("res/blusher.png"));
  SetImageTexture(blusher);
  SetTextureBounds(FrameBounds{395, 520, 489, 209});
  return FaceMakeupFilter::Init();
}

}  // namespace gpupixel
