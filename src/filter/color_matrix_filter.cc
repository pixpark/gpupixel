/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "gpupixel/filter/color_matrix_filter.h"
#include "core/gpupixel_context.h"
namespace gpupixel {

#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_ANDROID)
const std::string kColorMatrixFragmentShaderString = R"(
    uniform sampler2D inputImageTexture; uniform lowp mat4 colorMatrix;
    uniform lowp float intensity;

    varying highp vec2 textureCoordinate;

    void main() {
      lowp vec4 textureColor = texture2D(inputImageTexture, textureCoordinate);
      lowp vec4 outputColor = textureColor * colorMatrix;

      gl_FragColor =
          (intensity * outputColor) + ((1.0 - intensity) * textureColor);
    })";
#elif defined(GPUPIXEL_MAC) || defined(GPUPIXEL_WIN) || defined(GPUPIXEL_LINUX)
const std::string kColorMatrixFragmentShaderString = R"(
    uniform sampler2D inputImageTexture; uniform mat4 colorMatrix;
    uniform float intensity;

    varying vec2 textureCoordinate;

    void main() {
      vec4 textureColor = texture2D(inputImageTexture, textureCoordinate);
      vec4 outputColor = textureColor * colorMatrix;

      gl_FragColor =
          (intensity * outputColor) + ((1.0 - intensity) * textureColor);
    })";
#endif

ColorMatrixFilter::ColorMatrixFilter()
    : intensity_factor_(1.0), color_matrix_(Matrix4::IDENTITY) {}

std::shared_ptr<ColorMatrixFilter> ColorMatrixFilter::Create() {
  auto ret = std::shared_ptr<ColorMatrixFilter>(new ColorMatrixFilter());
  gpupixel::GPUPixelContext::GetInstance()->SyncRunWithContext([&] {
    if (ret && !ret->Init()) {
      ret.reset();
    }
  });
  return ret;
}

bool ColorMatrixFilter::Init() {
  if (!Filter::InitWithFragmentShaderString(kColorMatrixFragmentShaderString)) {
    return false;
  }

  RegisterProperty("intensity", intensity_factor_,
                   "The percentage of color applied by color matrix with range "
                   "between 0 and 1.",
                   [this](float& intensity) {
                     if (intensity > 1.0) {
                       intensity = 1.0;
                     } else if (intensity < 0.0) {
                       intensity = 0.0;
                     }
                     setIntensity(1.0);
                   });

  // todo register paoperty of color matrix

  return true;
}

bool ColorMatrixFilter::DoRender(bool updateSinks) {
  filter_program_->SetUniformValue("intensity", intensity_factor_);
  filter_program_->SetUniformValue("colorMatrix", color_matrix_);
  return Filter::DoRender(updateSinks);
}

}  // namespace gpupixel
