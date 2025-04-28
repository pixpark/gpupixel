/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "gpupixel/filter/filter_group.h"
#include "gpupixel/filter/grayscale_filter.h"
#include "gpupixel/filter/nearby_sampling3x3_filter.h"
#include "gpupixel/gpupixel_define.h"

namespace gpupixel {

class _SobelEdgeDetectionFilter;

class GPUPIXEL_API SobelEdgeDetectionFilter : public FilterGroup {
 public:
  static std::shared_ptr<SobelEdgeDetectionFilter> Create();
  ~SobelEdgeDetectionFilter();
  bool Init();

 protected:
  SobelEdgeDetectionFilter();

  std::shared_ptr<GrayscaleFilter> grayscale_filter_;
  std::shared_ptr<_SobelEdgeDetectionFilter> sobel_edge_detection_filter_;

  float edge_strength_;
};

class GPUPIXEL_API _SobelEdgeDetectionFilter : public NearbySampling3x3Filter {
 public:
  static std::shared_ptr<_SobelEdgeDetectionFilter> Create();
  bool Init();
  virtual bool DoRender(bool updateSinks = true) override;

  void setEdgeStrength(float edge_strength);

 protected:
  _SobelEdgeDetectionFilter() {};

  float edge_strength_;
};

}  // namespace gpupixel
