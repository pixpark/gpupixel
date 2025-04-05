/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "bilateral_filter.h"
#include "gpupixel_context.h"
namespace gpupixel {

const std::string kBilateralBlurVertexShaderString = R"(
    attribute vec4 position; attribute vec4 inputTextureCoordinate;

    const int GAUSSIAN_SAMPLES = 9;

    uniform float texelSpacingU;
    uniform float texelSpacingV;

    varying vec2 blurCoordinates[GAUSSIAN_SAMPLES];

    void main() {
      gl_Position = position;
      vec2 texelSpacing = vec2(texelSpacingU, texelSpacingV);
      for (int i = 0; i < GAUSSIAN_SAMPLES; i++) {
        blurCoordinates[i] =
            inputTextureCoordinate.xy +
            texelSpacing * float((i - ((GAUSSIAN_SAMPLES - 1) / 2)));
      }
    })";

#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_ANDROID)
const std::string kBilateralBlurFragmentShaderString = R"(
    uniform sampler2D inputImageTexture;

    const lowp int GAUSSIAN_SAMPLES = 9;

    varying highp vec2 blurCoordinates[GAUSSIAN_SAMPLES];

    uniform mediump float distanceNormalizationFactor;

    void main() {
      lowp vec4 centralColor;
      lowp float gaussianWeightTotal;
      lowp vec4 sum;
      lowp vec4 sampleColor;
      lowp float distanceFromCentralColor;
      lowp float gaussianWeight;

      centralColor = texture2D(inputImageTexture, blurCoordinates[4]);
      gaussianWeightTotal = 0.18;
      sum = centralColor * 0.18;

      sampleColor = texture2D(inputImageTexture, blurCoordinates[0]);
      distanceFromCentralColor =
          min(distance(centralColor, sampleColor) * distanceNormalizationFactor,
              1.0);
      gaussianWeight = 0.05 * (1.0 - distanceFromCentralColor);
      gaussianWeightTotal += gaussianWeight;
      sum += sampleColor * gaussianWeight;

      sampleColor = texture2D(inputImageTexture, blurCoordinates[1]);
      distanceFromCentralColor =
          min(distance(centralColor, sampleColor) * distanceNormalizationFactor,
              1.0);
      gaussianWeight = 0.09 * (1.0 - distanceFromCentralColor);
      gaussianWeightTotal += gaussianWeight;
      sum += sampleColor * gaussianWeight;

      sampleColor = texture2D(inputImageTexture, blurCoordinates[2]);
      distanceFromCentralColor =
          min(distance(centralColor, sampleColor) * distanceNormalizationFactor,
              1.0);
      gaussianWeight = 0.12 * (1.0 - distanceFromCentralColor);
      gaussianWeightTotal += gaussianWeight;
      sum += sampleColor * gaussianWeight;

      sampleColor = texture2D(inputImageTexture, blurCoordinates[3]);
      distanceFromCentralColor =
          min(distance(centralColor, sampleColor) * distanceNormalizationFactor,
              1.0);
      gaussianWeight = 0.15 * (1.0 - distanceFromCentralColor);
      gaussianWeightTotal += gaussianWeight;
      sum += sampleColor * gaussianWeight;

      sampleColor = texture2D(inputImageTexture, blurCoordinates[5]);
      distanceFromCentralColor =
          min(distance(centralColor, sampleColor) * distanceNormalizationFactor,
              1.0);
      gaussianWeight = 0.15 * (1.0 - distanceFromCentralColor);
      gaussianWeightTotal += gaussianWeight;
      sum += sampleColor * gaussianWeight;

      sampleColor = texture2D(inputImageTexture, blurCoordinates[6]);
      distanceFromCentralColor =
          min(distance(centralColor, sampleColor) * distanceNormalizationFactor,
              1.0);
      gaussianWeight = 0.12 * (1.0 - distanceFromCentralColor);
      gaussianWeightTotal += gaussianWeight;
      sum += sampleColor * gaussianWeight;

      sampleColor = texture2D(inputImageTexture, blurCoordinates[7]);
      distanceFromCentralColor =
          min(distance(centralColor, sampleColor) * distanceNormalizationFactor,
              1.0);
      gaussianWeight = 0.09 * (1.0 - distanceFromCentralColor);
      gaussianWeightTotal += gaussianWeight;
      sum += sampleColor * gaussianWeight;

      sampleColor = texture2D(inputImageTexture, blurCoordinates[8]);
      distanceFromCentralColor =
          min(distance(centralColor, sampleColor) * distanceNormalizationFactor,
              1.0);
      gaussianWeight = 0.05 * (1.0 - distanceFromCentralColor);
      gaussianWeightTotal += gaussianWeight;
      sum += sampleColor * gaussianWeight;

      gl_FragColor = sum / gaussianWeightTotal;
    })";
#elif defined(GPUPIXEL_MAC) || defined(GPUPIXEL_WIN) || defined(GPUPIXEL_LINUX)
const std::string kBilateralBlurFragmentShaderString = R"(
    uniform sampler2D inputImageTexture;

    const int GAUSSIAN_SAMPLES = 9;

    varying vec2 blurCoordinates[GAUSSIAN_SAMPLES];

    uniform float distanceNormalizationFactor;

    void main() {
      vec4 centralColor;
      float gaussianWeightTotal;
      vec4 sum;
      vec4 sampleColor;
      float distanceFromCentralColor;
      float gaussianWeight;

      centralColor = texture2D(inputImageTexture, blurCoordinates[4]);
      gaussianWeightTotal = 0.18;
      sum = centralColor * 0.18;

      sampleColor = texture2D(inputImageTexture, blurCoordinates[0]);
      distanceFromCentralColor =
          min(distance(centralColor, sampleColor) * distanceNormalizationFactor,
              1.0);
      gaussianWeight = 0.05 * (1.0 - distanceFromCentralColor);
      gaussianWeightTotal += gaussianWeight;
      sum += sampleColor * gaussianWeight;

      sampleColor = texture2D(inputImageTexture, blurCoordinates[1]);
      distanceFromCentralColor =
          min(distance(centralColor, sampleColor) * distanceNormalizationFactor,
              1.0);
      gaussianWeight = 0.09 * (1.0 - distanceFromCentralColor);
      gaussianWeightTotal += gaussianWeight;
      sum += sampleColor * gaussianWeight;

      sampleColor = texture2D(inputImageTexture, blurCoordinates[2]);
      distanceFromCentralColor =
          min(distance(centralColor, sampleColor) * distanceNormalizationFactor,
              1.0);
      gaussianWeight = 0.12 * (1.0 - distanceFromCentralColor);
      gaussianWeightTotal += gaussianWeight;
      sum += sampleColor * gaussianWeight;

      sampleColor = texture2D(inputImageTexture, blurCoordinates[3]);
      distanceFromCentralColor =
          min(distance(centralColor, sampleColor) * distanceNormalizationFactor,
              1.0);
      gaussianWeight = 0.15 * (1.0 - distanceFromCentralColor);
      gaussianWeightTotal += gaussianWeight;
      sum += sampleColor * gaussianWeight;

      sampleColor = texture2D(inputImageTexture, blurCoordinates[5]);
      distanceFromCentralColor =
          min(distance(centralColor, sampleColor) * distanceNormalizationFactor,
              1.0);
      gaussianWeight = 0.15 * (1.0 - distanceFromCentralColor);
      gaussianWeightTotal += gaussianWeight;
      sum += sampleColor * gaussianWeight;

      sampleColor = texture2D(inputImageTexture, blurCoordinates[6]);
      distanceFromCentralColor =
          min(distance(centralColor, sampleColor) * distanceNormalizationFactor,
              1.0);
      gaussianWeight = 0.12 * (1.0 - distanceFromCentralColor);
      gaussianWeightTotal += gaussianWeight;
      sum += sampleColor * gaussianWeight;

      sampleColor = texture2D(inputImageTexture, blurCoordinates[7]);
      distanceFromCentralColor =
          min(distance(centralColor, sampleColor) * distanceNormalizationFactor,
              1.0);
      gaussianWeight = 0.09 * (1.0 - distanceFromCentralColor);
      gaussianWeightTotal += gaussianWeight;
      sum += sampleColor * gaussianWeight;

      sampleColor = texture2D(inputImageTexture, blurCoordinates[8]);
      distanceFromCentralColor =
          min(distance(centralColor, sampleColor) * distanceNormalizationFactor,
              1.0);
      gaussianWeight = 0.05 * (1.0 - distanceFromCentralColor);
      gaussianWeightTotal += gaussianWeight;
      sum += sampleColor * gaussianWeight;

      gl_FragColor = sum / gaussianWeightTotal;
    })";
#endif
BilateralMonoFilter::BilateralMonoFilter(Type type)
    : type_(type),
      texel_spacing_multiplier_(4.0),
      distance_normalization_factor_(8.0) {}

std::shared_ptr<BilateralMonoFilter> BilateralMonoFilter::Create(
    Type type /* = HORIZONTAL*/) {
  auto ret =
      std::shared_ptr<BilateralMonoFilter>(new BilateralMonoFilter(type));
  if (!ret || !ret->Init()) {
    ret.reset();
  }
  return ret;
}

bool BilateralMonoFilter::Init() {
  if (Filter::InitWithShaderString(kBilateralBlurVertexShaderString,
                                   kBilateralBlurFragmentShaderString)) {
    return true;
  }
  return false;
}

bool BilateralMonoFilter::DoRender(bool updateSinks) {
  std::shared_ptr<GPUPixelFramebuffer> inputFramebuffer =
      input_framebuffers_.begin()->second.frame_buffer;
  RotationMode inputRotation =
      input_framebuffers_.begin()->second.rotation_mode;

  if (rotationSwapsSize(inputRotation)) {
    if (type_ == HORIZONTAL) {
      filter_program_->SetUniformValue("texelSpacingU", (float)0.0);
      filter_program_->SetUniformValue(
          "texelSpacingV",
          (float)(texel_spacing_multiplier_ / framebuffer_->GetWidth()));
    } else {
      filter_program_->SetUniformValue(
          "texelSpacingU",
          (float)(texel_spacing_multiplier_ / framebuffer_->GetHeight()));
      filter_program_->SetUniformValue("texelSpacingV", (float)0.0);
    }
  } else {
    if (type_ == HORIZONTAL) {
      filter_program_->SetUniformValue(
          "texelSpacingU",
          (float)(texel_spacing_multiplier_ / framebuffer_->GetWidth()));
      filter_program_->SetUniformValue("texelSpacingV", (float)0.0);
    } else {
      filter_program_->SetUniformValue("texelSpacingU", (float)0.0);
      filter_program_->SetUniformValue(
          "texelSpacingV",
          (float)(texel_spacing_multiplier_ / framebuffer_->GetHeight()));
    }
  }

  filter_program_->SetUniformValue("distanceNormalizationFactor",
                                   distance_normalization_factor_);
  return Filter::DoRender(updateSinks);
}

void BilateralMonoFilter::SetTexelSpacingMultiplier(float multiplier) {
  texel_spacing_multiplier_ = multiplier;
}

void BilateralMonoFilter::setDistanceNormalizationFactor(float value) {
  distance_normalization_factor_ = value;
}

BilateralFilter::BilateralFilter()
    : horizontal_blur_filter_(nullptr), vertical_blur_filter_(nullptr) {}

BilateralFilter::~BilateralFilter() {}

std::shared_ptr<BilateralFilter> BilateralFilter::Create() {
  auto ret = std::shared_ptr<BilateralFilter>(new BilateralFilter());
  gpupixel::GPUPixelContext::GetInstance()->SyncRunWithContext([&] {
    if (ret && !ret->Init()) {
      ret.reset();
    }
  });
  return ret;
}

bool BilateralFilter::Init() {
  if (!FilterGroup::Init()) {
    return false;
  }

  horizontal_blur_filter_ =
      BilateralMonoFilter::Create(BilateralMonoFilter::HORIZONTAL);
  vertical_blur_filter_ =
      BilateralMonoFilter::Create(BilateralMonoFilter::VERTICAL);
  horizontal_blur_filter_->AddSink(vertical_blur_filter_);
  AddFilter(horizontal_blur_filter_);

  RegisterProperty("texelSpacingMultiplier", 4.0,
                   "The texel spacing multiplier.",
                   [this](float& texelSpacingMultiplier) {
                     SetTexelSpacingMultiplier(texelSpacingMultiplier);
                   });

  RegisterProperty(
      "distanceNormalizationFactor", 8.0, "The distance normalization factor.",
      [this](float& distanceNormalizationFactor) {
        setDistanceNormalizationFactor(distanceNormalizationFactor);
      });

  return true;
}

void BilateralFilter::SetTexelSpacingMultiplier(float multiplier) {
  horizontal_blur_filter_->SetTexelSpacingMultiplier(multiplier);
  vertical_blur_filter_->SetTexelSpacingMultiplier(multiplier);
}

void BilateralFilter::setDistanceNormalizationFactor(float value) {
  horizontal_blur_filter_->setDistanceNormalizationFactor(value);
  vertical_blur_filter_->setDistanceNormalizationFactor(value);
}
}  // namespace gpupixel
