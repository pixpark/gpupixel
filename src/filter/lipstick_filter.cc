/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "gpupixel/filter/lipstick_filter.h"
#include "core/gpupixel_context.h"
#include "gpupixel/source/source_image.h"
#include "utils/util.h"
namespace gpupixel {

LipstickFilter::LipstickFilter() {}

std::shared_ptr<LipstickFilter> LipstickFilter::Create() {
  auto ret = std::shared_ptr<LipstickFilter>(new LipstickFilter());
  gpupixel::GPUPixelContext::GetInstance()->SyncRunWithContext([&] {
    if (ret && !ret->Init()) {
      ret.reset();
    }
  });
  return ret;
}

bool LipstickFilter::Init() {
  auto mouth = SourceImage::Create(Util::GetResourcePath("res/mouth.png"));
  SetImageTexture(mouth);
  SetTextureBounds(FrameBounds{502.5, 710, 262.5, 167.5});
  return FaceMakeupFilter::Init();
}

}  // namespace gpupixel
