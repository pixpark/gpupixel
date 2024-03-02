/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "smooth_toon_filter.h"

NS_GPUPIXEL_BEGIN

REGISTER_FILTER_CLASS(SmoothToonFilter)

SmoothToonFilter::SmoothToonFilter() : _gaussianBlurFilter(0), _toonFilter(0) {}

SmoothToonFilter::~SmoothToonFilter() {}

std::shared_ptr<SmoothToonFilter> SmoothToonFilter::create() {
  auto ret = std::shared_ptr<SmoothToonFilter>(new SmoothToonFilter());
  if (ret && !ret->init()) {
    ret.reset();
  }
  return ret;
}

bool SmoothToonFilter::init() {
  if (!FilterGroup::init()) {
    return false;
  }
  _gaussianBlurFilter = GaussianBlurFilter::create();
  _toonFilter = ToonFilter::create();
  _gaussianBlurFilter->addTarget(_toonFilter);
  addFilter(_gaussianBlurFilter);

  _blurRadius = 2.0;
  setBlurRadius(_blurRadius);
  registerProperty("blurRadius", _blurRadius, "",
                   [this](int& blurRadius) { setBlurRadius(blurRadius); });

  _toonThreshold = 0.2;
  registerProperty(
      "toonThreshold", _toonThreshold,
      "The threshold at which to apply the edges",
      [this](float& toonThreshold) { setToonThreshold(toonThreshold); });

  _toonQuantizationLevels = 10.0;
  registerProperty("toonQuantizationLevels", _toonQuantizationLevels,
                   "The levels of quantization for the posterization of colors "
                   "within the scene",
                   [this](float& toonQuantizationLevels) {
                     setToonQuantizationLevels(toonQuantizationLevels);
                   });

  return true;
}

void SmoothToonFilter::setBlurRadius(int blurRadius) {
  _blurRadius = blurRadius;
  _gaussianBlurFilter->setRadius(_blurRadius);
}

void SmoothToonFilter::setToonThreshold(float toonThreshold) {
  _toonThreshold = toonThreshold;
  _toonFilter->setThreshold(_toonThreshold);
}

void SmoothToonFilter::setToonQuantizationLevels(float toonQuantizationLevels) {
  _toonQuantizationLevels = toonQuantizationLevels;
  _toonFilter->setQuantizatinLevels(_toonQuantizationLevels);
}

NS_GPUPIXEL_END
