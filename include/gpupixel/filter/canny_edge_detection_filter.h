/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "gpupixel/filter/directional_non_maximum_suppression_filter.h"
#include "gpupixel/filter/directional_sobel_edge_detection_filter.h"
#include "gpupixel/filter/filter_group.h"
#include "gpupixel/filter/grayscale_filter.h"
#include "gpupixel/filter/single_component_gaussian_blur_filter.h"
#include "gpupixel/filter/weak_pixel_inclusion_filter.h"
#include "gpupixel/gpupixel_define.h"

namespace gpupixel {
class GPUPIXEL_API CannyEdgeDetectionFilter : public FilterGroup {
 public:
  static std::shared_ptr<CannyEdgeDetectionFilter> Create();
  ~CannyEdgeDetectionFilter();
  bool Init();

 protected:
  CannyEdgeDetectionFilter();

  std::shared_ptr<GrayscaleFilter> grayscale_filter_;
  std::shared_ptr<SingleComponentGaussianBlurFilter> blur_filter_;
  std::shared_ptr<DirectionalSobelEdgeDetectionFilter> edge_detection_filter_;
  std::shared_ptr<DirectionalNonMaximumSuppressionFilter>
      non_maximum_suppression_filter_;
  std::shared_ptr<WeakPixelInclusionFilter> weak_pixel_inclusion_filter_;
};

}  // namespace gpupixel
