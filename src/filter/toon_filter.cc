/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "toon_filter.h"
#include "gpupixel_context.h"
namespace gpupixel {

const std::string kToonFragmentShaderString = R"(
    precision mediump float; uniform sampler2D inputImageTexture;
    uniform float threshold;
    uniform float quantizationLevels;

    varying vec2 textureCoordinate;
    varying vec2 vLeftTexCoord;
    varying vec2 vRightTexCoord;

    varying vec2 vTopTexCoord;
    varying vec2 vTopLeftTexCoord;
    varying vec2 vTopRightTexCoord;

    varying vec2 vBottomTexCoord;
    varying vec2 vBottomLeftTexCoord;
    varying vec2 vBottomRightTexCoord;

    void main() {
      float bottomLeftIntensity =
          texture2D(inputImageTexture, vBottomLeftTexCoord).r;
      float topRightIntensity =
          texture2D(inputImageTexture, vTopRightTexCoord).r;
      float topLeftIntensity = texture2D(inputImageTexture, vTopLeftTexCoord).r;
      float bottomRightIntensity =
          texture2D(inputImageTexture, vBottomRightTexCoord).r;
      float leftIntensity = texture2D(inputImageTexture, vLeftTexCoord).r;
      float rightIntensity = texture2D(inputImageTexture, vRightTexCoord).r;
      float bottomIntensity = texture2D(inputImageTexture, vBottomTexCoord).r;
      float topIntensity = texture2D(inputImageTexture, vTopTexCoord).r;
      float h = -topLeftIntensity - 2.0 * topIntensity - topRightIntensity +
                bottomLeftIntensity + 2.0 * bottomIntensity +
                bottomRightIntensity;
      float v = -bottomLeftIntensity - 2.0 * leftIntensity - topLeftIntensity +
                bottomRightIntensity + 2.0 * rightIntensity + topRightIntensity;

      float mag = length(vec2(h, v));

      vec4 color = texture2D(inputImageTexture, textureCoordinate);
      vec3 posterizedImageColor =
          (floor(color.rgb * quantizationLevels) + 0.5) / quantizationLevels;

      float thresholdTest = 1.0 - step(threshold, mag);

      gl_FragColor = vec4(posterizedImageColor * thresholdTest, color.a);
    })";

std::shared_ptr<ToonFilter> ToonFilter::Create() {
  auto ret = std::shared_ptr<ToonFilter>(new ToonFilter());
  gpupixel::GPUPixelContext::GetInstance()->SyncRunWithContext([&] {
    if (ret && !ret->Init()) {
      ret.reset();
    }
  });
  return ret;
}

bool ToonFilter::Init() {
  if (!InitWithFragmentShaderString(kToonFragmentShaderString)) {
    return false;
  }

  threshold_ = 0.2;
  RegisterProperty("threshold", threshold_,
                   "The threshold at which to apply the edges",
                   [this](float& threshold) { setThreshold(threshold); });

  quantization_levels_ = 10.0;
  RegisterProperty("quantizationLevels", quantization_levels_,
                   "The levels of quantization for the posterization of colors "
                   "within the scene",
                   [this](float& quantizationLevels) {
                     setQuantizatinLevels(quantizationLevels);
                   });

  return true;
}

void ToonFilter::setThreshold(float threshold) {
  threshold_ = threshold;
}

void ToonFilter::setQuantizatinLevels(float quantizationLevels) {
  quantization_levels_ = quantizationLevels;
}

bool ToonFilter::DoRender(bool updateSinks) {
  filter_program_->SetUniformValue("threshold", threshold_);
  filter_program_->SetUniformValue("quantizationLevels", quantization_levels_);
  return NearbySampling3x3Filter::DoRender(updateSinks);
}

}  // namespace gpupixel
