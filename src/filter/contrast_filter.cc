/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "contrast_filter.h"

namespace gpupixel {

REGISTER_FILTER_CLASS(ContrastFilter)

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

  _contrast = 1.0;
  RegisterProperty("contrast", _contrast,
                   "The contrast of the image. Contrast ranges from 0.0 to 4.0 "
                   "(max contrast), with 1.0 as the normal level",
                   [this](float& contrast) { setContrast(contrast); });

  return true;
}

void ContrastFilter::setContrast(float contrast) {
  _contrast = contrast;
  if (_contrast > 4.0) {
    _contrast = 4.0;
  } else if (_contrast < 0.0) {
    _contrast = 0.0;
  }
}

bool ContrastFilter::DoRender(bool updateSinks) {
  _filterProgram->SetUniformValue("contrast", _contrast);
  return Filter::DoRender(updateSinks);
}

} // namespace gpupixel