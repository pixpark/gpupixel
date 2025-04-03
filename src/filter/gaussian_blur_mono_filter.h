/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "filter_group.h"
#include "gpupixel_macros.h"

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
  void setTexelSpacingMultiplier(float value);

 protected:
  GaussianBlurMonoFilter(Type type = HORIZONTAL);
  Type _type;
  int _radius;
  float _sigma;

  float verticalTexelSpacing_ = 1.0;
  float horizontalTexelSpacing_ = 1.0;

 private:
  virtual std::string _generateVertexShaderString(int radius, float sigma);
  virtual std::string _generateFragmentShaderString(int radius, float sigma);

  virtual std::string _generateOptimizedVertexShaderString(int radius,
                                                           float sigma);
  virtual std::string _generateOptimizedFragmentShaderString(int radius,
                                                             float sigma);
};

}
