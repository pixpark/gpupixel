/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "FilterGroup.h"
#include "GPUPixelDef.h"

NS_GPUPIXEL_BEGIN

class BilateralMonoFilter : public Filter {
 public:
  enum Type { HORIZONTAL, VERTICAL };

  static std::shared_ptr<BilateralMonoFilter> create(Type type = HORIZONTAL);
  bool init();

  virtual bool proceed(bool bUpdateTargets = true,
                       int64_t frameTime = 0) override;

  void setTexelSpacingMultiplier(float multiplier);
  void setDistanceNormalizationFactor(float value);

 protected:
  BilateralMonoFilter(Type type);
  Type _type;
  float _texelSpacingMultiplier;
  float _distanceNormalizationFactor;
};

class BilateralFilter : public FilterGroup {
 public:
  virtual ~BilateralFilter();

  static std::shared_ptr<BilateralFilter> create();
  bool init();

  void setTexelSpacingMultiplier(float multiplier);
  void setDistanceNormalizationFactor(float value);

 protected:
  BilateralFilter();

 private:
  // friend BilateralMonoFilter;
  std::shared_ptr<BilateralMonoFilter> _hBlurFilter;
  std::shared_ptr<BilateralMonoFilter> _vBlurFilter;
};

NS_GPUPIXEL_END
