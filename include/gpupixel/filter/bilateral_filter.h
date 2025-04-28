/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "gpupixel/filter/filter_group.h"
#include "gpupixel/gpupixel_define.h"

namespace gpupixel {
class GPUPIXEL_API BilateralMonoFilter : public Filter {
 public:
  enum Type { HORIZONTAL, VERTICAL };

  static std::shared_ptr<BilateralMonoFilter> Create(Type type = HORIZONTAL);
  bool Init();

  virtual bool DoRender(bool updateSinks = true) override;

  void SetTexelSpacingMultiplier(float multiplier);
  void setDistanceNormalizationFactor(float value);

 protected:
  BilateralMonoFilter(Type type);
  Type type_;
  float texel_spacing_multiplier_;
  float distance_normalization_factor_;
};

class GPUPIXEL_API BilateralFilter : public FilterGroup {
 public:
  virtual ~BilateralFilter();

  static std::shared_ptr<BilateralFilter> Create();
  bool Init();

  void SetTexelSpacingMultiplier(float multiplier);
  void setDistanceNormalizationFactor(float value);

 protected:
  BilateralFilter();

 private:
  // friend BilateralMonoFilter;
  std::shared_ptr<BilateralMonoFilter> horizontal_blur_filter_;
  std::shared_ptr<BilateralMonoFilter> vertical_blur_filter_;
};

}  // namespace gpupixel
