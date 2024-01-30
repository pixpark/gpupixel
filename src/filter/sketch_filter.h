/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "filter_group.h"
#include "gpupixel_macros.h"
#include "grayscale_filter.h"
#include "nearby_sampling3x3_filter.h"

NS_GPUPIXEL_BEGIN

// Sketch filter is just the Sobel edge detection filter with the colors
// inverted.

class _SketchFilter;

class SketchFilter : public FilterGroup {
 public:
  ~SketchFilter();
  static std::shared_ptr<SketchFilter> create();
  bool init();

 protected:
  SketchFilter();

  std::shared_ptr<GrayscaleFilter> _grayscaleFilter;
  std::shared_ptr<_SketchFilter> _sketchFilter;

  float _edgeStrength;
};

class _SketchFilter : public NearbySampling3x3Filter {
 public:
  static std::shared_ptr<_SketchFilter> create();
  bool init();
  virtual bool proceed(bool bUpdateTargets = true,
                       int64_t frameTime = 0) override;

  void setEdgeStrength(float edgeStrength);

 protected:
  _SketchFilter(){};

  float _edgeStrength;
};

NS_GPUPIXEL_END
