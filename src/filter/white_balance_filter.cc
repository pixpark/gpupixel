/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "white_balance_filter.h"
#include "gpupixel_context.h"
namespace gpupixel {

const std::string kWhiteBalanceFragmentShaderString = R"(
    uniform sampler2D inputImageTexture; uniform lowp float temperature;
    uniform lowp float tint;
    varying highp vec2 textureCoordinate;
    const lowp vec3 warmFilter = vec3(0.93, 0.54, 0.0);
    const mediump mat3 RGBtoYIQ =
        mat3(0.299, 0.587, 0.114,
             0.596, -0.274, -0.322,
             0.212, -0.523, 0.311);
    const mediump mat3 YIQtoRGB =
        mat3(1.0, 0.956, 0.621,
             1.0, -0.272, -0.647,
             1.0, -1.105, 1.702);

    void main() {
      lowp vec4 color = texture2D(inputImageTexture, textureCoordinate);
      mediump vec3 yiq = RGBtoYIQ * color.rgb;  // adjusting tint
      yiq.b = clamp(yiq.b + tint * 0.5226 * 0.1, -0.5226, 0.5226);
      lowp vec3 rgb = YIQtoRGB * yiq;
      lowp vec3 processed = vec3(
          (rgb.r < 0.5
               ? (2.0 * rgb.r * warmFilter.r)
               : (1.0 - 2.0 * (1.0 - rgb.r) *
                            (1.0 - warmFilter.r))),  // adjusting temperature
          (rgb.g < 0.5 ? (2.0 * rgb.g * warmFilter.g)
                       : (1.0 - 2.0 * (1.0 - rgb.g) * (1.0 - warmFilter.g))),
          (rgb.b < 0.5 ? (2.0 * rgb.b * warmFilter.b)
                       : (1.0 - 2.0 * (1.0 - rgb.b) * (1.0 - warmFilter.b))));

      gl_FragColor = vec4(mix(rgb, processed, temperature), color.a);
    })";

std::shared_ptr<WhiteBalanceFilter> WhiteBalanceFilter::Create() {
  auto ret = std::shared_ptr<WhiteBalanceFilter>(new WhiteBalanceFilter());
  gpupixel::GPUPixelContext::GetInstance()->SyncRunWithContext([&] {
    if (ret && !ret->Init()) {
      ret.reset();
    }
  });
  return ret;
}

bool WhiteBalanceFilter::Init() {
  if (!InitWithFragmentShaderString(kWhiteBalanceFragmentShaderString)) {
    return false;
  }

  setTemperature(5000.0);
  RegisterProperty(
      "temperature", 5000.0,
      "Adjustment of color temperature (in degrees Kelvin) in terms of what "
      "an "
      "image was effectively shot in. This means higher Kelvin values will "
      "warm the image, while lower values will cool it.",
      [this](float& temperature) { setTemperature(temperature); });

  setTint(0.0);
  RegisterProperty("tint", tint_, "adjust tint to compensate",
                   [this](float& tint) { setTint(tint); });

  return true;
}

void WhiteBalanceFilter::setTemperature(float temperature) {
  temperature_ = temperature < 5000 ? 0.0004 * (temperature - 5000.0)
                                    : 0.00006 * (temperature - 5000.0);
}

void WhiteBalanceFilter::setTint(float tint) {
  tint_ = tint / 100.0;
}

bool WhiteBalanceFilter::DoRender(bool updateSinks) {
  filter_program_->SetUniformValue("temperature", temperature_);
  filter_program_->SetUniformValue("tint", tint_);
  return Filter::DoRender(updateSinks);
}

}  // namespace gpupixel
