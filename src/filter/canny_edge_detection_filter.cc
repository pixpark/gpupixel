/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "canny_edge_detection_filter.h"
#include "gpupixel_context.h"
namespace gpupixel {

CannyEdgeDetectionFilter::CannyEdgeDetectionFilter()
    : grayscale_filter_(0),
      blur_filter_(0),
      edge_detection_filter_(0),
      non_maximum_suppression_filter_(0),
      weak_pixel_inclusion_filter_(0) {}

CannyEdgeDetectionFilter::~CannyEdgeDetectionFilter() {}

std::shared_ptr<CannyEdgeDetectionFilter> CannyEdgeDetectionFilter::Create() {
  auto ret =
      std::shared_ptr<CannyEdgeDetectionFilter>(new CannyEdgeDetectionFilter());
  gpupixel::GPUPixelContext::GetInstance()->SyncRunWithContext([&] {
    if (ret && !ret->Init()) {
      ret.reset();
    }
  });
  return ret;
}

bool CannyEdgeDetectionFilter::Init() {
  if (!FilterGroup::Init()) {
    return false;
  }

  // 1. convert image to luminance
  grayscale_filter_ = GrayscaleFilter::Create();

  // 2. apply a varialbe Gaussian blur
  blur_filter_ = SingleComponentGaussianBlurFilter::Create();

  // 3. soble edge detection
  edge_detection_filter_ = DirectionalSobelEdgeDetectionFilter::Create();

  // 4. apply non-maximum suppression
  non_maximum_suppression_filter_ =
      DirectionalNonMaximumSuppressionFilter::Create();

  // 5. include weak pixels to complete edges
  weak_pixel_inclusion_filter_ = WeakPixelInclusionFilter::Create();

  grayscale_filter_->AddSink(blur_filter_)
      ->AddSink(edge_detection_filter_)
      ->AddSink(non_maximum_suppression_filter_)
      ->AddSink(weak_pixel_inclusion_filter_);
  AddFilter(grayscale_filter_);

  return true;
}

}  // namespace gpupixel
