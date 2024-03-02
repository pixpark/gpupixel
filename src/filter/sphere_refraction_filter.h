/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "filter.h"
#include "gpupixel_macros.h"

NS_GPUPIXEL_BEGIN

class SphereRefractionFilter : public Filter {
 public:
  static std::shared_ptr<SphereRefractionFilter> create();
  bool init();
  virtual bool proceed(bool bUpdateTargets = true,
                       int64_t frameTime = 0) override;

  void setPositionX(float x);
  void setPositionY(float y);
  void setRadius(float radius);
  void setRefractiveIndex(float refractiveIndex);

 protected:
  SphereRefractionFilter(){};

  // The position about which to apply the distortion, with a default of (0.5,
  // 0.5)
  Vector2 _position;

  // The radius of the distortion, ranging from 0.0 to 1.0, with a default of
  // 0.25
  float _radius;

  // The index of refraction for the sphere, with a default of 0.71
  float _refractiveIndex;
};

NS_GPUPIXEL_END
