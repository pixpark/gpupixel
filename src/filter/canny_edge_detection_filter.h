/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "directional_non_maximum_suppression_filter.h"
#include "directional_sobel_edge_detection_filter.h"
#include "filter_group.h"
#include "gpupixel_macros.h"
#include "grayscale_filter.h"
#include "single_component_gaussian_blur_filter.h"
#include "weak_pixel_inclusion_filter.h"

NS_GPUPIXEL_BEGIN

class CannyEdgeDetectionFilter : public FilterGroup {
 public:
  static std::shared_ptr<CannyEdgeDetectionFilter> create();
  ~CannyEdgeDetectionFilter();
  bool init();

 protected:
  CannyEdgeDetectionFilter();

  std::shared_ptr<GrayscaleFilter> _grayscaleFilter;
  std::shared_ptr<SingleComponentGaussianBlurFilter> _blurFilter;
  std::shared_ptr<DirectionalSobelEdgeDetectionFilter> _edgeDetectionFilter;
  std::shared_ptr<DirectionalNonMaximumSuppressionFilter>
      _nonMaximumSuppressionFilter;
  std::shared_ptr<WeakPixelInclusionFilter> _weakPixelInclusionFilter;
};

NS_GPUPIXEL_END
