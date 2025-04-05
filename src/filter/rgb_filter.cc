/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "rgb_filter.h"
#include "gpupixel_context.h"
namespace gpupixel {

const std::string kRGBFragmentShaderString = R"(
    uniform sampler2D inputImageTexture; uniform highp float redAdjustment;
    uniform highp float greenAdjustment;
    uniform highp float blueAdjustment;
    varying highp vec2 textureCoordinate;

    void main() {
      lowp vec4 color = texture2D(inputImageTexture, textureCoordinate);
      gl_FragColor = vec4(color.r * redAdjustment, color.g * greenAdjustment,
                          color.b * blueAdjustment, color.a);
    })";

std::shared_ptr<RGBFilter> RGBFilter::Create() {
  auto ret = std::shared_ptr<RGBFilter>(new RGBFilter());
  gpupixel::GPUPixelContext::GetInstance()->SyncRunWithContext([&] {
    if (ret && !ret->Init()) {
      ret.reset();
    }
  });
  return ret;
}

bool RGBFilter::Init() {
  if (!InitWithFragmentShaderString(kRGBFragmentShaderString)) {
    return false;
  }

  red_adjustment_ = 1.0;
  green_adjustment_ = 1.0;
  blue_adjustment_ = 1.0;

  RegisterProperty(
      "redAdjustment", red_adjustment_,
      "The red adjustment of the image.The range is from 0.0 up, with 1.0 as "
      "the default.",
      [this](float& redAdjustment) { setRedAdjustment(redAdjustment); });

  RegisterProperty(
      "greenAdjustment", green_adjustment_,
      "The green adjustment of the image.The range is from 0.0 "
      "up, with 1.0 as "
      "the default.",
      [this](float& greenAdjustment) { setGreenAdjustment(greenAdjustment); });

  RegisterProperty(
      "blueAdjustment", blue_adjustment_,
      "The blue adjustment of the image.The range is from 0.0 up, with 1.0 "
      "as "
      "the default.",
      [this](float& blueAdjustment) { setBlueAdjustment(blueAdjustment); });

  return true;
}

void RGBFilter::setRedAdjustment(float redAdjustment) {
  red_adjustment_ = redAdjustment;
  if (red_adjustment_ < 0.0) {
    red_adjustment_ = 0.0;
  }
}

void RGBFilter::setGreenAdjustment(float greenAdjustment) {
  green_adjustment_ = greenAdjustment;
  if (green_adjustment_ < 0.0) {
    green_adjustment_ = 0.0;
  }
}

void RGBFilter::setBlueAdjustment(float blueAdjustment) {
  blue_adjustment_ = blueAdjustment;
  if (blue_adjustment_ < 0.0) {
    blue_adjustment_ = 0.0;
  }
}
bool RGBFilter::DoRender(bool updateSinks) {
  filter_program_->SetUniformValue("redAdjustment", red_adjustment_);
  filter_program_->SetUniformValue("greenAdjustment", green_adjustment_);
  filter_program_->SetUniformValue("blueAdjustment", blue_adjustment_);
  return Filter::DoRender(updateSinks);
}

}  // namespace gpupixel
