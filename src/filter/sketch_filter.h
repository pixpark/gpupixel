/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "filter_group.h"
#include "gpupixel_macros.h"
#include "grayscale_filter.h"
#include "nearby_sampling3x3_filter.h"

namespace gpupixel {

// Sketch filter is just the Sobel edge detection filter with the colors
// inverted.

class _SketchFilter;

class GPUPIXEL_API SketchFilter : public FilterGroup {
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

class GPUPIXEL_API _SketchFilter : public NearbySampling3x3Filter {
 public:
  static std::shared_ptr<_SketchFilter> create();
  bool init();
  virtual bool doRender(bool updateSinks = true) override;

  void setEdgeStrength(float edgeStrength);

 protected:
  _SketchFilter(){};

  float _edgeStrength;
};

}
