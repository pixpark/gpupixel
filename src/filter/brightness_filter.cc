/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "gpupixel/filter/brightness_filter.h"
#include "core/gpupixel_context.h"
namespace gpupixel {

#if defined(GPUPIXEL_GLES_SHADER) || defined(GPUPIXEL_MAC)
const std::string kBrightnessFragmentShaderString = R"(
    uniform sampler2D inputImageTexture; uniform lowp float brightness_factor;
    varying highp vec2 textureCoordinate;

    void main() {
      lowp vec4 color = texture2D(inputImageTexture, textureCoordinate);
      gl_FragColor = vec4((color.rgb + vec3(brightness_factor)), color.a);
    })";
#elif defined(GPUPIXEL_WIN) || defined(GPUPIXEL_MAC) || defined(GPUPIXEL_LINUX)
const std::string kBrightnessFragmentShaderString = R"(
    uniform sampler2D inputImageTexture; uniform float brightness_factor;
    varying vec2 textureCoordinate;

    void main() {
      vec4 color = texture2D(inputImageTexture, textureCoordinate);
      gl_FragColor = vec4((color.rgb + vec3(brightness_factor)), color.a);
    })";
#endif

std::shared_ptr<BrightnessFilter> BrightnessFilter::Create(
    float brightness /* = 0.0*/) {
  auto ret = std::shared_ptr<BrightnessFilter>(new BrightnessFilter());
  gpupixel::GPUPixelContext::GetInstance()->SyncRunWithContext([&] {
    if (ret && !ret->Init(brightness)) {
      ret.reset();
    }
  });
  return ret;
}

bool BrightnessFilter::Init(float brightness) {
  if (!InitWithFragmentShaderString(kBrightnessFragmentShaderString)) {
    return false;
  }

  brightness_factor_ = 0.01;
  RegisterProperty("brightness_factor", brightness_factor_,
                   "The brightness of filter with range between -1 and 1.",
                   [this](float& brightness) { setBrightness(brightness); });

  return true;
}

void BrightnessFilter::setBrightness(float brightness) {
  brightness_factor_ = brightness;
  if (brightness_factor_ > 1.0) {
    brightness_factor_ = 1.0;
  } else if (brightness_factor_ < -1.0) {
    brightness_factor_ = -1.0;
  }
}

bool BrightnessFilter::DoRender(bool updateSinks) {
  filter_program_->SetUniformValue("brightness_factor", brightness_factor_);
  return Filter::DoRender(updateSinks);
}

}  // namespace gpupixel
