/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "filter_group.h"
#include "gpupixel_define.h"

namespace gpupixel {
class GPUPIXEL_API BilateralMonoFilter : public Filter {
 public:
  enum Type { HORIZONTAL, VERTICAL };

  static std::shared_ptr<BilateralMonoFilter> Create(Type type = HORIZONTAL);
  bool Init();

  virtual bool DoRender(bool updateSinks = true) override;

  void setTexelSpacingMultiplier(float multiplier);
  void setDistanceNormalizationFactor(float value);

 protected:
  BilateralMonoFilter(Type type);
  Type _type;
  float _texelSpacingMultiplier;
  float _distanceNormalizationFactor;
};

class GPUPIXEL_API BilateralFilter : public FilterGroup {
 public:
  virtual ~BilateralFilter();

  static std::shared_ptr<BilateralFilter> Create();
  bool Init();

  void setTexelSpacingMultiplier(float multiplier);
  void setDistanceNormalizationFactor(float value);

 protected:
  BilateralFilter();

 private:
  // friend BilateralMonoFilter;
  std::shared_ptr<BilateralMonoFilter> _hBlurFilter;
  std::shared_ptr<BilateralMonoFilter> _vBlurFilter;
};

}
