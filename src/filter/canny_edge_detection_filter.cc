/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "canny_edge_detection_filter.h"

NS_GPUPIXEL_BEGIN

REGISTER_FILTER_CLASS(CannyEdgeDetectionFilter)

CannyEdgeDetectionFilter::CannyEdgeDetectionFilter()
    : _grayscaleFilter(0),
      _blurFilter(0),
      _edgeDetectionFilter(0),
      _nonMaximumSuppressionFilter(0),
      _weakPixelInclusionFilter(0) {}

CannyEdgeDetectionFilter::~CannyEdgeDetectionFilter() {}

std::shared_ptr<CannyEdgeDetectionFilter> CannyEdgeDetectionFilter::create() {
  auto ret =
      std::shared_ptr<CannyEdgeDetectionFilter>(new CannyEdgeDetectionFilter());
  if (ret && !ret->init()) {
    ret.reset();
  }
  return ret;
}

bool CannyEdgeDetectionFilter::init() {
  if (!FilterGroup::init()) {
    return false;
  }

  // 1. convert image to luminance
  _grayscaleFilter = GrayscaleFilter::create();

  // 2. apply a varialbe Gaussian blur
  _blurFilter = SingleComponentGaussianBlurFilter::create();

  // 3. soble edge detection
  _edgeDetectionFilter = DirectionalSobelEdgeDetectionFilter::create();

  // 4. apply non-maximum suppression
  _nonMaximumSuppressionFilter =
      DirectionalNonMaximumSuppressionFilter::create();

  // 5. include weak pixels to complete edges
  _weakPixelInclusionFilter = WeakPixelInclusionFilter::create();

  _grayscaleFilter->addTarget(_blurFilter)
      ->addTarget(_edgeDetectionFilter)
      ->addTarget(_nonMaximumSuppressionFilter)
      ->addTarget(_weakPixelInclusionFilter);
  addFilter(_grayscaleFilter);

  return true;
}

NS_GPUPIXEL_END
