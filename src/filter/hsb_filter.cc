/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "hsb_filter.h"

NS_GPUPIXEL_BEGIN

REGISTER_FILTER_CLASS(HSBFilter)

/* Matrix algorithms adapted from
 http://www.graficaobscura.com/matrix/index.html

 Note about luminance vector values below from that page:
 Where rwgt is 0.3086, gwgt is 0.6094, and bwgt is 0.0820. This is the luminance
 vector. Notice here that we do not use the standard NTSC weights of 0.299,
 0.587, and 0.114. The NTSC weights are only applicable to RGB colors in a
 gamma 2.2 color space. For linear RGB colors the values above are better.
 */
// #define RLUM (0.3086f)
// #define GLUM (0.6094f)
// #define BLUM (0.0820f)

/* This is the vector value from the PDF specification, and may be closer to
 * what Photoshop uses */
#define RLUM (0.3f)
#define GLUM (0.59f)
#define BLUM (0.11f)

std::shared_ptr<HSBFilter> HSBFilter::create() {
  auto ret = std::shared_ptr<HSBFilter>(new HSBFilter());
  if (ret && !ret->init()) {
    ret.reset();
  }
  return ret;
}

bool HSBFilter::init() {
  if (!ColorMatrixFilter::init()) {
    return false;
  }
  reset();

  return true;
}

void HSBFilter::reset() {
  setColorMatrix(Matrix4::IDENTITY);
}

void HSBFilter::rotateHue(float h) {
  // todo
}

void HSBFilter::adjustSaturation(float sat) {
  Matrix4 sMat;
  float rwgt = RLUM;
  float gwgt = GLUM;
  float bwgt = BLUM;

  sMat.m[0] = (1.0 - sat) * rwgt + sat;
  sMat.m[1] = (1.0 - sat) * rwgt;
  sMat.m[2] = (1.0 - sat) * rwgt;
  sMat.m[3] = 0.0;
  sMat.m[4] = (1.0 - sat) * gwgt;
  sMat.m[5] = (1.0 - sat) * gwgt + sat;
  sMat.m[6] = (1.0 - sat) * gwgt;
  sMat.m[7] = 0.0;
  sMat.m[8] = (1.0 - sat) * bwgt;
  sMat.m[9] = (1.0 - sat) * bwgt;
  sMat.m[10] = (1.0 - sat) * bwgt + sat;
  sMat.m[11] = 0.0;
  sMat.m[12] = 0.0;
  sMat.m[13] = 0.0;
  sMat.m[14] = 0.0;
  sMat.m[15] = 1.0;

  saturationMatrix_ = sMat;

  _colorMatrix = Matrix4::IDENTITY;
  _colorMatrix *= brightnessMatrix_;
  _colorMatrix = saturationMatrix_ * _colorMatrix;
}

void HSBFilter::adjustBrightness(float b) {
  brightnessMatrix_ = Matrix4::IDENTITY;
  brightnessMatrix_ *= b;

  _colorMatrix = Matrix4::IDENTITY;
  _colorMatrix *= brightnessMatrix_;
  _colorMatrix = saturationMatrix_ * _colorMatrix;
}

NS_GPUPIXEL_END
