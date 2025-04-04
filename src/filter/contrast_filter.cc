/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "contrast_filter.h"

namespace gpupixel {

const std::string kContrastFragmentShaderString = R"(
    uniform sampler2D inputImageTexture; uniform lowp float contrast;
    varying highp vec2 textureCoordinate;

    void main() {
      lowp vec4 color = texture2D(inputImageTexture, textureCoordinate);
      gl_FragColor =
          vec4(((color.rgb - vec3(0.5)) * contrast + vec3(0.5)), color.a);
    })";

std::shared_ptr<ContrastFilter> ContrastFilter::Create() {
  auto ret = std::shared_ptr<ContrastFilter>(new ContrastFilter());
  if (ret && !ret->Init()) {
    ret.reset();
  }
  return ret;
}

bool ContrastFilter::Init() {
  if (!InitWithFragmentShaderString(kContrastFragmentShaderString)) {
    return false;
  }

  contrast_factor_ = 1.0;
  RegisterProperty("contrast", contrast_factor_,
                   "The contrast of the image. Contrast ranges from 0.0 to 4.0 "
                   "(max contrast), with 1.0 as the normal level",
                   [this](float& contrast) { setContrast(contrast); });

  return true;
}

void ContrastFilter::setContrast(float contrast) {
  contrast_factor_ = contrast;
  if (contrast_factor_ > 4.0) {
    contrast_factor_ = 4.0;
  } else if (contrast_factor_ < 0.0) {
    contrast_factor_ = 0.0;
  }
}

bool ContrastFilter::DoRender(bool updateSinks) {
  filter_program_->SetUniformValue("contrast", contrast_factor_);
  return Filter::DoRender(updateSinks);
}

}  // namespace gpupixel
