/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "posterize_filter.h"

namespace gpupixel {

REGISTER_FILTER_CLASS(PosterizeFilter)

#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_ANDROID)
const std::string kPosterizeFragmentShaderString = R"(
    uniform sampler2D inputImageTexture; uniform highp float colorLevels;
    varying highp vec2 textureCoordinate;

    void main() {
      lowp vec4 color = texture2D(inputImageTexture, textureCoordinate);

      gl_FragColor = floor((color * colorLevels) + vec4(0.5)) / colorLevels;
    })";
#elif defined(GPUPIXEL_MAC) || defined(GPUPIXEL_WIN) || defined(GPUPIXEL_LINUX)
const std::string kPosterizeFragmentShaderString = R"(
    uniform sampler2D inputImageTexture; uniform float colorLevels;
    varying vec2 textureCoordinate;

    void main() {
      vec4 color = texture2D(inputImageTexture, textureCoordinate);

      gl_FragColor = floor((color * colorLevels) + vec4(0.5)) / colorLevels;
    })";
#endif

std::shared_ptr<PosterizeFilter> PosterizeFilter::Create() {
  auto ret = std::shared_ptr<PosterizeFilter>(new PosterizeFilter());
  if (ret && !ret->Init()) {
    // todo zhaoyou
  }
  return ret;
}

bool PosterizeFilter::Init() {
  if (!InitWithFragmentShaderString(kPosterizeFragmentShaderString)) {
    return false;
  }

  _colorLevels = 10;
  RegisterProperty("colorLevels", _colorLevels,
                   "The number of color levels to reduce the image space to. "
                   "This ranges from 1 to 256, with a default of 10.",
                   [this](int& colorLevels) { setColorLevels(colorLevels); });

  return true;
}

void PosterizeFilter::setColorLevels(int colorLevels) {
  _colorLevels = colorLevels;
  if (_colorLevels > 256) {
    _colorLevels = 256;
  } else if (_colorLevels < 1) {
    _colorLevels = 1;
  }
}

bool PosterizeFilter::DoRender(bool updateSinks) {
  _filterProgram->SetUniformValue("colorLevels", (float)_colorLevels);
  return Filter::DoRender(updateSinks);
}

} // namespace gpupixel