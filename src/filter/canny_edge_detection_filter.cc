/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "canny_edge_detection_filter.h"

namespace gpupixel {

REGISTER_FILTER_CLASS(CannyEdgeDetectionFilter)

CannyEdgeDetectionFilter::CannyEdgeDetectionFilter()
    : _grayscaleFilter(0),
      _blurFilter(0),
      _edgeDetectionFilter(0),
      _nonMaximumSuppressionFilter(0),
      _weakPixelInclusionFilter(0) {}

CannyEdgeDetectionFilter::~CannyEdgeDetectionFilter() {}

std::shared_ptr<CannyEdgeDetectionFilter> CannyEdgeDetectionFilter::Create() {
  auto ret =
      std::shared_ptr<CannyEdgeDetectionFilter>(new CannyEdgeDetectionFilter());
  if (ret && !ret->Init()) {
    ret.reset();
  }
  return ret;
}

bool CannyEdgeDetectionFilter::Init() {
  if (!FilterGroup::Init()) {
    return false;
  }

  // 1. convert image to luminance
  _grayscaleFilter = GrayscaleFilter::Create();

  // 2. apply a varialbe Gaussian blur
  _blurFilter = SingleComponentGaussianBlurFilter::Create();

  // 3. soble edge detection
  _edgeDetectionFilter = DirectionalSobelEdgeDetectionFilter::Create();

  // 4. apply non-maximum suppression
  _nonMaximumSuppressionFilter =
      DirectionalNonMaximumSuppressionFilter::Create();

  // 5. include weak pixels to complete edges
  _weakPixelInclusionFilter = WeakPixelInclusionFilter::Create();

  _grayscaleFilter->AddSink(_blurFilter)
      ->AddSink(_edgeDetectionFilter)
      ->AddSink(_nonMaximumSuppressionFilter)
      ->AddSink(_weakPixelInclusionFilter);
  addFilter(_grayscaleFilter);

  return true;
}

}
