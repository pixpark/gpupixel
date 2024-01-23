/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "gpupixel_macros.h"
#include "sphere_refraction_filter.h"

NS_GPUPIXEL_BEGIN

class GlassSphereFilter : public SphereRefractionFilter {
 public:
  static std::shared_ptr<GlassSphereFilter> create();
  bool init();

 protected:
  GlassSphereFilter(){};
};

NS_GPUPIXEL_END
