/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "color_invert_filter.h"

namespace gpupixel {

const std::string kColorInvertFragmentShaderString = R"(

    uniform sampler2D inputImageTexture; varying highp vec2 textureCoordinate;

    void main() {
      lowp vec4 color = texture2D(inputImageTexture, textureCoordinate);
      gl_FragColor = vec4((1.0 - color.rgb), color.a);
    })";

std::shared_ptr<ColorInvertFilter> ColorInvertFilter::Create() {
  auto ret = std::shared_ptr<ColorInvertFilter>(new ColorInvertFilter());
  if (ret && !ret->Init()) {
    ret.reset();
  }
  return ret;
}

bool ColorInvertFilter::Init() {
  if (!Filter::InitWithFragmentShaderString(kColorInvertFragmentShaderString)) {
    return false;
  }
  return true;
}

bool ColorInvertFilter::DoRender(bool updateSinks) {
  return Filter::DoRender(updateSinks);
}

}  // namespace gpupixel
