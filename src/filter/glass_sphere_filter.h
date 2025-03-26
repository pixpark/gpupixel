/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "gpupixel_macros.h"
#include "sphere_refraction_filter.h"

namespace gpupixel {
class GPUPIXEL_API GlassSphereFilter : public SphereRefractionFilter {
 public:
  static std::shared_ptr<GlassSphereFilter> create();
  bool init();

 protected:
  GlassSphereFilter(){};
};

}
