/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "FilterGroup.h"
#include "GPUPixelDef.h"
#include "GrayscaleFilter.h"
#include "NearbySampling3x3Filter.h"

NS_GPUPIXEL_BEGIN

class _SobelEdgeDetectionFilter;

class SobelEdgeDetectionFilter : public FilterGroup {
 public:
  static std::shared_ptr<SobelEdgeDetectionFilter> create();
  ~SobelEdgeDetectionFilter();
  bool init();

 protected:
  SobelEdgeDetectionFilter();

  std::shared_ptr<GrayscaleFilter> _grayscaleFilter;
  std::shared_ptr<_SobelEdgeDetectionFilter> _sobelEdgeDetectionFilter;

  float _edgeStrength;
};

class _SobelEdgeDetectionFilter : public NearbySampling3x3Filter {
 public:
  static std::shared_ptr<_SobelEdgeDetectionFilter> create();
  bool init();
  virtual bool proceed(bool bUpdateTargets = true,
                       int64_t frameTime = 0) override;

  void setEdgeStrength(float edgeStrength);

 protected:
  _SobelEdgeDetectionFilter(){};

  float _edgeStrength;
};

NS_GPUPIXEL_END
