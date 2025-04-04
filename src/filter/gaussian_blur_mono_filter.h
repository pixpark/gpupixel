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
class GPUPIXEL_API GaussianBlurMonoFilter : public Filter {
 public:
  enum Type { HORIZONTAL, VERTICAL };

  static std::shared_ptr<GaussianBlurMonoFilter> Create(Type type = HORIZONTAL,
                                                        int radius = 4,
                                                        float sigma = 2.0);
  bool Init(int radius, float sigma);

  void SetRadius(int radius);
  void setSigma(float sigma);

  virtual bool DoRender(bool updateSinks = true) override;
  void SetTexelSpacingMultiplier(float value);

 protected:
  GaussianBlurMonoFilter(Type type = HORIZONTAL);
  Type type_;
  int radius_;
  float sigma_;

  float vertical_texel_spacing_ = 1.0;
  float horizontal_texel_spacing_ = 1.0;

  virtual std::string GenerateOptimizedVertexShaderString(int radius,
                                                          float sigma);
  virtual std::string GenerateOptimizedFragmentShaderString(int radius,
                                                            float sigma);

 private:
  virtual std::string GenerateVertexShaderString(int radius, float sigma);
  virtual std::string GenerateFragmentShaderString(int radius, float sigma);
};

}  // namespace gpupixel
