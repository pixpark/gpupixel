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

class GaussianBlurMonoFilter : public Filter {
 public:
  enum Type { HORIZONTAL, VERTICAL };

  static std::shared_ptr<GaussianBlurMonoFilter> create(Type type = HORIZONTAL,
                                                        int radius = 4,
                                                        float sigma = 2.0);
  bool init(int radius, float sigma);

  void setRadius(int radius);
  void setSigma(float sigma);

  virtual bool proceed(bool bUpdateTargets = true,
                       int64_t frameTime = 0) override;
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

NS_GPUPIXEL_END
