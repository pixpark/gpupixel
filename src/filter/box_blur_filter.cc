/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "box_blur_filter.h"
#include "gpupixel_context.h"
namespace gpupixel {

BoxBlurFilter::BoxBlurFilter()
    : horizontal_blur_filter_(nullptr), vertical_blur_filter_(nullptr) {}

BoxBlurFilter::~BoxBlurFilter() {}

std::shared_ptr<BoxBlurFilter> BoxBlurFilter::Create(int radius /* = 4*/,
                                                     float sigma /* = 2.0*/) {
  auto ret = std::shared_ptr<BoxBlurFilter>(new BoxBlurFilter());
  gpupixel::GPUPixelContext::GetInstance()->SyncRunWithContext([&] {
    if (ret && !ret->Init(radius, sigma)) {
      ret.reset();
    }
  });
  return ret;
}

bool BoxBlurFilter::Init(int radius, float sigma) {
  if (!FilterGroup::Init()) {
    return false;
  }

  horizontal_blur_filter_ =
      BoxMonoBlurFilter::Create(GaussianBlurMonoFilter::HORIZONTAL, 4, 0.0);
  vertical_blur_filter_ =
      BoxMonoBlurFilter::Create(GaussianBlurMonoFilter::VERTICAL, 4, 0.0);
  horizontal_blur_filter_->AddSink(vertical_blur_filter_);
  AddFilter(horizontal_blur_filter_);

  RegisterProperty("radius", 4, "", [this](int& radius) { SetRadius(radius); });

  RegisterProperty("sigma", 0.0, "", [this](float& sigma) { setSigma(sigma); });

  return true;
}

void BoxBlurFilter::SetRadius(int radius) {
  horizontal_blur_filter_->SetRadius(radius);
  vertical_blur_filter_->SetRadius(radius);
}

void BoxBlurFilter::setSigma(float sigma) {
  horizontal_blur_filter_->setSigma(sigma);
  vertical_blur_filter_->setSigma(sigma);
}

void BoxBlurFilter::SetTexelSpacingMultiplier(float value) {
  horizontal_blur_filter_->SetTexelSpacingMultiplier(value);
  vertical_blur_filter_->SetTexelSpacingMultiplier(value);
}

}  // namespace gpupixel
