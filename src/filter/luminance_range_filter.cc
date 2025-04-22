/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "gpupixel/filter/luminance_range_filter.h"
#include "core/gpupixel_context.h"
namespace gpupixel {

#if defined(GPUPIXEL_GLES_SHADER)
const std::string kLuminanceRangeFragmentShaderString = R"(
    uniform sampler2D inputImageTexture;
    uniform lowp float rangeReductionFactor;
    varying highp vec2 textureCoordinate;

    // Values from "Graphics Shaders: Theory and Practice" by Bailey and
    // Cunningham
    const mediump vec3 luminanceWeighting = vec3(0.2125, 0.7154, 0.0721);

    void main() {
      lowp vec4 color = texture2D(inputImageTexture, textureCoordinate);
      mediump float luminance = dot(color.rgb, luminanceWeighting);
      mediump float luminanceRatio = ((0.5 - luminance) * rangeReductionFactor);
      gl_FragColor = vec4((color.rgb) + (luminanceRatio), color.a);
    })";
#elif defined(GPUPIXEL_GL_SHADER)
const std::string kLuminanceRangeFragmentShaderString = R"(
    uniform sampler2D inputImageTexture; uniform float rangeReductionFactor;
    varying vec2 textureCoordinate;

    // Values from "Graphics Shaders: Theory and Practice" by Bailey and
    // Cunningham
    const vec3 luminanceWeighting = vec3(0.2125, 0.7154, 0.0721);

    void main() {
      vec4 color = texture2D(inputImageTexture, textureCoordinate);
      float luminance = dot(color.rgb, luminanceWeighting);
      float luminanceRatio = ((0.5 - luminance) * rangeReductionFactor);
      gl_FragColor = vec4((color.rgb) + (luminanceRatio), color.a);
    })";
#endif

std::shared_ptr<LuminanceRangeFilter> LuminanceRangeFilter::Create() {
  auto ret = std::shared_ptr<LuminanceRangeFilter>(new LuminanceRangeFilter());
  gpupixel::GPUPixelContext::GetInstance()->SyncRunWithContext([&] {
    if (ret && !ret->Init()) {
      ret.reset();
    }
  });
  return ret;
}

bool LuminanceRangeFilter::Init() {
  if (!InitWithFragmentShaderString(kLuminanceRangeFragmentShaderString)) {
    return false;
  }

  range_reduction_factor_ = 0.6;
  RegisterProperty("rangeReductionFactor", range_reduction_factor_,
                   "The degree to reduce the luminance range, from 0.0 to 1.0. "
                   "Default is 0.6.",
                   [this](float& rangeReductionFactor) {
                     setRangeReductionFactor(rangeReductionFactor);
                   });

  return true;
}

void LuminanceRangeFilter::setRangeReductionFactor(float rangeReductionFactor) {
  range_reduction_factor_ = rangeReductionFactor;
  if (range_reduction_factor_ > 1.0) {
    range_reduction_factor_ = 1.0;
  } else if (range_reduction_factor_ < 0.0) {
    range_reduction_factor_ = 0.0;
  }
}

bool LuminanceRangeFilter::DoRender(bool updateSinks) {
  filter_program_->SetUniformValue("rangeReductionFactor",
                                   range_reduction_factor_);
  return Filter::DoRender(updateSinks);
}

}  // namespace gpupixel
