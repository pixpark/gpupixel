/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "DirectionalNonMaximumSuppressionFilter.h"
#include "DirectionalSobelEdgeDetectionFilter.h"
#include "FilterGroup.h"
#include "GPUPixelDef.h"
#include "GrayscaleFilter.h"
#include "SingleComponentGaussianBlurFilter.h"
#include "WeakPixelInclusionFilter.h"

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
