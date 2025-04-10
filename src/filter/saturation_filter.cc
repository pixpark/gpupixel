/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "gpupixel/filter/saturation_filter.h"
#include "core/gpupixel_context.h"
namespace gpupixel {

const std::string kSaturationFragmentShaderString = R"(
    uniform sampler2D inputImageTexture; uniform lowp float saturation;
    varying highp vec2 textureCoordinate;

    // Values from "Graphics Shaders: Theory and Practice" by Bailey and
    // Cunningham
    const mediump vec3 luminanceWeighting = vec3(0.2125, 0.7154, 0.0721);

    void main() {
      lowp vec4 color = texture2D(inputImageTexture, textureCoordinate);
      lowp float luminance = dot(color.rgb, luminanceWeighting);
      lowp vec3 greyScaleColor = vec3(luminance);

      gl_FragColor = vec4(mix(greyScaleColor, color.rgb, saturation), color.a);
    })";

std::shared_ptr<SaturationFilter> SaturationFilter::Create() {
  auto ret = std::shared_ptr<SaturationFilter>(new SaturationFilter());
  gpupixel::GPUPixelContext::GetInstance()->SyncRunWithContext([&] {
    if (ret && !ret->Init()) {
      ret.reset();
    }
  });
  return ret;
}

bool SaturationFilter::Init() {
  if (!InitWithFragmentShaderString(kSaturationFragmentShaderString)) {
    return false;
  }

  saturation_ = 1.0;
  RegisterProperty(
      "saturation", saturation_,
      "The saturation of an image. Saturation ranges from 0.0 (fully "
      "desaturated) to 2.0 (max saturation), with 1.0 as the normal level",
      [this](float& saturation) { setSaturation(saturation); });

  return true;
}

void SaturationFilter::setSaturation(float saturation) {
  saturation_ = saturation;
  if (saturation_ > 2.0) {
    saturation_ = 2.0;
  } else if (saturation_ < 0.0) {
    saturation_ = 0.0;
  }
}

bool SaturationFilter::DoRender(bool updateSinks) {
  filter_program_->SetUniformValue("saturation", saturation_);
  return Filter::DoRender(updateSinks);
}

}  // namespace gpupixel
