/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "gpupixel/filter/blusher_filter.h"
#include "core/gpupixel_context.h"
#include "gpupixel/source/source_image.h"
#include "utils/util.h"
namespace gpupixel {

BlusherFilter::BlusherFilter() {}

std::shared_ptr<BlusherFilter> BlusherFilter::Create() {
  auto ret = std::shared_ptr<BlusherFilter>(new BlusherFilter());
  gpupixel::GPUPixelContext::GetInstance()->SyncRunWithContext([&] {
    if (ret && !ret->Init()) {
      ret.reset();
    }
  });
  return ret;
}

bool BlusherFilter::Init() {
  auto blusher = SourceImage::Create(Util::GetResourcePath("res/blusher.png"));
  SetImageTexture(blusher);
  SetTextureBounds(FrameBounds{395, 520, 489, 209});
  return FaceMakeupFilter::Init();
}

}  // namespace gpupixel
