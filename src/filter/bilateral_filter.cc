/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "bilateral_filter.h"

NS_GPUPIXEL_BEGIN

REGISTER_FILTER_CLASS(BilateralMonoFilter)

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
    precision mediump float;
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
    : _type(type),
      _texelSpacingMultiplier(4.0),
      _distanceNormalizationFactor(8.0) {}

std::shared_ptr<BilateralMonoFilter> BilateralMonoFilter::create(
    Type type /* = HORIZONTAL*/) {
  auto ret =
      std::shared_ptr<BilateralMonoFilter>(new BilateralMonoFilter(type));
  if (!ret || !ret->init()) {
    ret.reset();
  }
  return ret;
}

bool BilateralMonoFilter::init() {
  if (Filter::initWithShaderString(kBilateralBlurVertexShaderString,
                                   kBilateralBlurFragmentShaderString)) {
    return true;
  }
  return false;
}

bool BilateralMonoFilter::proceed(bool bUpdateTargets, int64_t frameTime) {
  std::shared_ptr<Framebuffer> inputFramebuffer =
      _inputFramebuffers.begin()->second.frameBuffer;
  RotationMode inputRotation = _inputFramebuffers.begin()->second.rotationMode;

  if (rotationSwapsSize(inputRotation)) {
    if (_type == HORIZONTAL) {
      _filterProgram->setUniformValue("texelSpacingU", (float)0.0);
      _filterProgram->setUniformValue(
          "texelSpacingV",
          (float)(_texelSpacingMultiplier / _framebuffer->getWidth()));
    } else {
      _filterProgram->setUniformValue(
          "texelSpacingU",
          (float)(_texelSpacingMultiplier / _framebuffer->getHeight()));
      _filterProgram->setUniformValue("texelSpacingV", (float)0.0);
    }
  } else {
    if (_type == HORIZONTAL) {
      _filterProgram->setUniformValue(
          "texelSpacingU",
          (float)(_texelSpacingMultiplier / _framebuffer->getWidth()));
      _filterProgram->setUniformValue("texelSpacingV", (float)0.0);
    } else {
      _filterProgram->setUniformValue("texelSpacingU", (float)0.0);
      _filterProgram->setUniformValue(
          "texelSpacingV",
          (float)(_texelSpacingMultiplier / _framebuffer->getHeight()));
    }
  }

  _filterProgram->setUniformValue("distanceNormalizationFactor",
                                  _distanceNormalizationFactor);
  return Filter::proceed(bUpdateTargets, frameTime);
}

void BilateralMonoFilter::setTexelSpacingMultiplier(float multiplier) {
  _texelSpacingMultiplier = multiplier;
}

void BilateralMonoFilter::setDistanceNormalizationFactor(float value) {
  _distanceNormalizationFactor = value;
}

REGISTER_FILTER_CLASS(BilateralFilter)

BilateralFilter::BilateralFilter() : _hBlurFilter(0), _vBlurFilter(0) {}

BilateralFilter::~BilateralFilter() {}

std::shared_ptr<BilateralFilter> BilateralFilter::create() {
  auto ret = std::shared_ptr<BilateralFilter>(new BilateralFilter());
  if (ret && !ret->init()) {
    ret.reset();
  }
  return ret;
}

bool BilateralFilter::init() {
  if (!FilterGroup::init()) {
    return false;
  }

  _hBlurFilter = BilateralMonoFilter::create(BilateralMonoFilter::HORIZONTAL);
  _vBlurFilter = BilateralMonoFilter::create(BilateralMonoFilter::VERTICAL);
  _hBlurFilter->addTarget(_vBlurFilter);
  addFilter(_hBlurFilter);

  registerProperty("texelSpacingMultiplier", 4.0,
                   "The texel spacing multiplier.",
                   [this](float& texelSpacingMultiplier) {
                     setTexelSpacingMultiplier(texelSpacingMultiplier);
                   });

  registerProperty(
      "distanceNormalizationFactor", 8.0, "The distance normalization factor.",
      [this](float& distanceNormalizationFactor) {
        setDistanceNormalizationFactor(distanceNormalizationFactor);
      });

  return true;
}

void BilateralFilter::setTexelSpacingMultiplier(float multiplier) {
  _hBlurFilter->setTexelSpacingMultiplier(multiplier);
  _vBlurFilter->setTexelSpacingMultiplier(multiplier);
}

void BilateralFilter::setDistanceNormalizationFactor(float value) {
  _hBlurFilter->setDistanceNormalizationFactor(value);
  _vBlurFilter->setDistanceNormalizationFactor(value);
}
NS_GPUPIXEL_END
