/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "gpupixel/filter/sphere_refraction_filter.h"
#include "gpupixel/gpupixel_define.h"

namespace gpupixel {
class GPUPIXEL_API GlassSphereFilter : public SphereRefractionFilter {
 public:
  static std::shared_ptr<GlassSphereFilter> Create();
  bool Init();

 protected:
  GlassSphereFilter() {};
};

}  // namespace gpupixel
