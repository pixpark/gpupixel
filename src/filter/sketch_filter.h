/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "filter_group.h"
#include "gpupixel_define.h"
#include "grayscale_filter.h"
#include "nearby_sampling3x3_filter.h"

namespace gpupixel {

// Sketch filter is just the Sobel edge detection filter with the colors
// inverted.

class _SketchFilter;

class GPUPIXEL_API SketchFilter : public FilterGroup {
 public:
  ~SketchFilter();
  static std::shared_ptr<SketchFilter> Create();
  bool Init();

 protected:
  SketchFilter();

  std::shared_ptr<GrayscaleFilter> grayscale_filter_;
  std::shared_ptr<_SketchFilter> sketch_filter_;

  float edge_strength_;
};

class GPUPIXEL_API _SketchFilter : public NearbySampling3x3Filter {
 public:
  static std::shared_ptr<_SketchFilter> Create();
  bool Init();
  virtual bool DoRender(bool updateSinks = true) override;

  void setEdgeStrength(float edge_strength);

 protected:
  _SketchFilter() {};

  float edge_strength_;
};

}  // namespace gpupixel
