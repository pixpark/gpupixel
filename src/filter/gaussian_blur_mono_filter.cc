/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "gaussian_blur_mono_filter.h"
#include <cmath>
#include "util.h"

namespace gpupixel {

GaussianBlurMonoFilter::GaussianBlurMonoFilter(Type type /* = HORIZONTAL*/)
    : type_(type), radius_(4), sigma_(2.0) {}

std::shared_ptr<GaussianBlurMonoFilter> GaussianBlurMonoFilter::Create(
    Type type /* = HORIZONTAL*/,
    int radius /* = 4*/,
    float sigma /* = 2.0*/) {
  auto ret =
      std::shared_ptr<GaussianBlurMonoFilter>(new GaussianBlurMonoFilter(type));
  if (ret && !ret->Init(radius, sigma)) {
    ret.reset();
  }
  return ret;
}

bool GaussianBlurMonoFilter::Init(int radius, float sigma) {
  if (Filter::InitWithShaderString(
          GenerateOptimizedVertexShaderString(radius, sigma),
          GenerateOptimizedFragmentShaderString(radius, sigma))) {
    return true;
  }
  return false;
}

void GaussianBlurMonoFilter::SetRadius(int radius) {
  if (radius == radius_) {
    return;
  }

  radius_ = radius;

  if (filter_program_) {
    delete filter_program_;
    filter_program_ = 0;
  }
  InitWithShaderString(GenerateOptimizedVertexShaderString(radius_, sigma_),
                       GenerateOptimizedFragmentShaderString(radius_, sigma_));
}

void GaussianBlurMonoFilter::setSigma(float sigma) {
  if (sigma == sigma_) {
    return;
  }

  sigma_ = round(sigma);

  int calculatedSampleRadius = 0;
  if (sigma_ >= 1)  // Avoid a divide-by-zero error here
  {
    // Calculate the number of pixels to sample from by setting a bottom limit
    // for the contribution of the outermost pixel
    float minimumWeightToFindEdgeOfSamplingArea = 1.0 / 256.0;
    calculatedSampleRadius =
        floor(sqrt(-2.0 * pow(sigma_, 2.0) *
                   log(minimumWeightToFindEdgeOfSamplingArea *
                       sqrt(2.0 * M_PI * pow(sigma_, 2.0)))));
    calculatedSampleRadius +=
        calculatedSampleRadius %
        2;  // There's nothing to gain from handling odd
            // radius sizes, due to the optimizations I use
  }
  radius_ = calculatedSampleRadius;

  if (filter_program_) {
    delete filter_program_;
    filter_program_ = 0;
  }
  InitWithShaderString(GenerateOptimizedVertexShaderString(radius_, sigma_),
                       GenerateOptimizedFragmentShaderString(radius_, sigma_));
}

bool GaussianBlurMonoFilter::DoRender(bool updateSinks) {
  RotationMode inputRotation =
      input_framebuffers_.begin()->second.rotation_mode;

  if (rotationSwapsSize(inputRotation)) {
    if (type_ == HORIZONTAL) {
      filter_program_->SetUniformValue("texelWidthOffset", (float)0.0);
      filter_program_->SetUniformValue(
          "texelHeightOffset",
          (float)(vertical_texel_spacing_ / framebuffer_->GetWidth()));
    } else {
      filter_program_->SetUniformValue(
          "texelWidthOffset",
          (float)(horizontal_texel_spacing_ / framebuffer_->GetHeight()));
      filter_program_->SetUniformValue("texelHeightOffset", (float)0.0);
    }
  } else {
    if (type_ == HORIZONTAL) {
      filter_program_->SetUniformValue(
          "texelWidthOffset",
          (float)(horizontal_texel_spacing_ / framebuffer_->GetWidth()));
      filter_program_->SetUniformValue("texelHeightOffset", (float)0.0);
    } else {
      filter_program_->SetUniformValue("texelWidthOffset", (float)0.0);
      filter_program_->SetUniformValue(
          "texelHeightOffset",
          (float)(vertical_texel_spacing_ / framebuffer_->GetHeight()));
    }
  }
  return Filter::DoRender(updateSinks);
}

void GaussianBlurMonoFilter::SetTexelSpacingMultiplier(float value) {
  vertical_texel_spacing_ = value;
  horizontal_texel_spacing_ = value;
}

std::string GaussianBlurMonoFilter::GenerateVertexShaderString(int radius,
                                                               float sigma) {
  if (radius < 1 || sigma <= 0.0) {
    return kDefaultVertexShader;
  }

  std::string shaderStr = Util::str_format(
      "\
        attribute vec4 position;\n\
        attribute vec4 inputTextureCoordinate;\n\
        uniform float texelWidthOffset;\n\
        uniform float texelHeightOffset;\n\
        varying vec2 blurCoordinates[%d];\n\
        void main()\n\
        {\n\
            gl_Position = position;\n\
            vec2 texelSpacing = vec2(texelWidthOffset, texelHeightOffset);\n\
        ",
      radius * 2 + 1);

  for (int i = 0; i < radius * 2 + 1; ++i) {
    int offsetFromCenter = i - radius;
    if (offsetFromCenter == 0) {
      shaderStr = shaderStr +
                  Util::str_format(
                      "blurCoordinates[%d] = inputTextureCoordinate.xy;\n", i);
    } else {
      shaderStr =
          shaderStr + Util::str_format(
                          "blurCoordinates[%d] = inputTextureCoordinate.xy + "
                          "texelSpacing * (%f);\n",
                          i, (float)offsetFromCenter);
    }
  }

  shaderStr += "}\n";

  return shaderStr;
}

std::string GaussianBlurMonoFilter::GenerateFragmentShaderString(int radius,
                                                                 float sigma) {
  if (radius < 1 || sigma <= 0.0) {
    return kDefaultFragmentShader;
  }

  float* standardGaussianWeights = new float[radius + 1];
  float sumOfWeights = 0.0;
  for (int i = 0; i < radius + 1; ++i) {
    standardGaussianWeights[i] = (1.0 / sqrt(2.0 * PI * pow(sigma, 2.0))) *
                                 exp(-pow(i, 2.0) / (2.0 * pow(sigma, 2.0)));

    if (i == 0) {
      sumOfWeights += standardGaussianWeights[i];
    } else {
      sumOfWeights += 2.0 * standardGaussianWeights[i];
    }
  }
  for (int i = 0; i < radius + 1; ++i) {
    standardGaussianWeights[i] = standardGaussianWeights[i] / sumOfWeights;
  }
#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_ANDROID)
  std::string shaderStr = Util::str_format(
      "\
           uniform sampler2D inputImageTexture;\n\
           varying highp vec2 blurCoordinates[%d];\n\
           void main()\n\
           {\n\
               gl_FragColor = vec4(0.0);\n",
      radius * 2 + 1);
#elif defined(GPUPIXEL_MAC) || defined(GPUPIXEL_WIN) || defined(GPUPIXEL_LINUX)
  std::string shaderStr = Util::str_format(
      "\
           uniform sampler2D inputImageTexture;\n\
           varying vec2 blurCoordinates[%d];\n\
           void main()\n\
           {\n\
               gl_FragColor = vec4(0.0);\n",
      radius * 2 + 1);
#endif
  for (int i = 0; i < radius * 2 + 1; ++i) {
    int offsetFromCenter = i - radius;
    shaderStr += Util::str_format(
        "gl_FragColor += texture2D(inputImageTexture, "
        "blurCoordinates[%d]) * %f;\n",
        i, standardGaussianWeights[abs(offsetFromCenter)]);
  }
  shaderStr += "}";

  delete[] standardGaussianWeights;
  standardGaussianWeights = 0;

  return shaderStr;
}

std::string GaussianBlurMonoFilter::GenerateOptimizedVertexShaderString(
    int radius,
    float sigma) {
  if (radius < 1 || sigma <= 0.0) {
    return kDefaultVertexShader;
  }

  // 1. generate the normal Gaussian weights for a given sigma
  float* standardGaussianWeights = new float[radius + 1];
  float sumOfWeights = 0.0;
  for (int i = 0; i < radius + 1; ++i) {
    standardGaussianWeights[i] = (1.0 / sqrt(2.0 * M_PI * pow(sigma, 2.0))) *
                                 exp(-pow(i, 2.0) / (2.0 * pow(sigma, 2.0)));
    if (i == 0) {
      sumOfWeights += standardGaussianWeights[i];
    } else {
      sumOfWeights += 2.0 * standardGaussianWeights[i];
    }
  }

  // 2. normalize these weights to prevent the clipping of the Gaussian curve at
  // the end of the discrete samples from reducing luminance
  for (int i = 0; i < radius + 1; ++i) {
    standardGaussianWeights[i] = standardGaussianWeights[i] / sumOfWeights;
  }

  // 3. From these weights we calculate the offsets to read interpolated values
  // from
  int numberOfOptimizedOffsets = fmin(radius / 2 + (radius % 2), 7);
  float* optimizedGaussianOffsets = new float[numberOfOptimizedOffsets];

  for (int i = 0; i < numberOfOptimizedOffsets; ++i) {
    GLfloat firstWeight = standardGaussianWeights[i * 2 + 1];
    GLfloat secondWeight = standardGaussianWeights[i * 2 + 2];

    GLfloat optimizedWeight = firstWeight + secondWeight;

    optimizedGaussianOffsets[i] =
        (firstWeight * (i * 2 + 1) + secondWeight * (i * 2 + 2)) /
        optimizedWeight;
  }

#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_ANDROID)
  std::string shaderStr = Util::str_format(
      "\
               attribute vec4 position;\n\
               attribute vec4 inputTextureCoordinate;\n\
               uniform float texelWidthOffset;\n\
               uniform float texelHeightOffset;\n\
               varying highp vec2 blurCoordinates[%d];\n\
               void main()\n\
               {\n\
               gl_Position = position;\n\
               vec2 texelSpacing = vec2(texelWidthOffset, texelHeightOffset);\n\
               ",
      numberOfOptimizedOffsets * 2 + 1);
#elif defined(GPUPIXEL_MAC) || defined(GPUPIXEL_WIN) || defined(GPUPIXEL_LINUX)
  std::string shaderStr = Util::str_format(
      "\
               attribute vec4 position;\n\
               attribute vec4 inputTextureCoordinate;\n\
               float texelWidthOffset;\n\
               float texelHeightOffset;\n\
               varying vec2 blurCoordinates[%d];\n\
               void main()\n\
               {\n\
               gl_Position = position;\n\
               vec2 texelSpacing = vec2(texelWidthOffset, texelHeightOffset);\n\
               ",
      numberOfOptimizedOffsets * 2 + 1);
#endif
  shaderStr =
      shaderStr +
      Util::str_format("blurCoordinates[0] = inputTextureCoordinate.xy;\n");
  for (int i = 0; i < numberOfOptimizedOffsets; ++i) {
    shaderStr =
        shaderStr +
        Util::str_format(
            "blurCoordinates[%d] = inputTextureCoordinate.xy + texelSpacing * (%f);\n\
            blurCoordinates[%d] = inputTextureCoordinate.xy - texelSpacing * (%f);",
            i * 2 + 1, optimizedGaussianOffsets[i], i * 2 + 2,
            optimizedGaussianOffsets[i]);
  }

  shaderStr += "}\n";

  delete[] standardGaussianWeights;
  delete[] optimizedGaussianOffsets;

  return shaderStr;
}

std::string GaussianBlurMonoFilter::GenerateOptimizedFragmentShaderString(
    int radius,
    float sigma) {
  if (radius < 1 || sigma <= 0.0) {
    return kDefaultFragmentShader;
  }

  // 1. generate the normal Gaussian weights for a given sigma
  float* standardGaussianWeights = new float[radius + 1];
  float sumOfWeights = 0.0;
  for (int i = 0; i < radius + 1; ++i) {
    standardGaussianWeights[i] = (1.0 / sqrt(2.0 * M_PI * pow(sigma, 2.0))) *
                                 exp(-pow(i, 2.0) / (2.0 * pow(sigma, 2.0)));
    if (i == 0) {
      sumOfWeights += standardGaussianWeights[i];
    } else {
      sumOfWeights += 2.0 * standardGaussianWeights[i];
    }
  }

  // 2. normalize these weights to prevent the clipping of the Gaussian curve at
  // the end of the discrete samples from reducing luminance
  for (int i = 0; i < radius + 1; ++i) {
    standardGaussianWeights[i] = standardGaussianWeights[i] / sumOfWeights;
  }

  // 3. From these weights we calculate the offsets to read interpolated values
  // from
  int trueNumberOfOptimizedOffsets = radius / 2 + (radius % 2);
  int numberOfOptimizedOffsets = fmin(trueNumberOfOptimizedOffsets, 7);
#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_ANDROID)
  std::string shaderStr = Util::str_format(
      "\
               uniform sampler2D inputImageTexture;\n\
               uniform highp float texelWidthOffset;\n\
               uniform highp float texelHeightOffset;\n\
               varying highp vec2 blurCoordinates[%d];\n\
               void main()\n\
               {\n\
               gl_FragColor = vec4(0.0);\n",
      numberOfOptimizedOffsets * 2 + 1);
#elif defined(GPUPIXEL_MAC) || defined(GPUPIXEL_WIN) || defined(GPUPIXEL_LINUX)
  std::string shaderStr = Util::str_format(
      "\
               uniform sampler2D inputImageTexture;\n\
               uniform float texelWidthOffset;\n\
               uniform float texelHeightOffset;\n\
               varying vec2 blurCoordinates[%d];\n\
               void main()\n\
               {\n\
               gl_FragColor = vec4(0.0);\n",
      numberOfOptimizedOffsets * 2 + 1);
#endif
  shaderStr += Util::str_format(
      "gl_FragColor += texture2D(inputImageTexture, "
      "blurCoordinates[0]) * %f;\n",
      standardGaussianWeights[0]);
  for (int i = 0; i < numberOfOptimizedOffsets; ++i) {
    float firstWeight = standardGaussianWeights[i * 2 + 1];
    float secondWeight = standardGaussianWeights[i * 2 + 2];
    float optimizedWeight = firstWeight + secondWeight;

    shaderStr += Util::str_format(
        "gl_FragColor += texture2D(inputImageTexture, "
        "blurCoordinates[%d]) * %f;\n",
        i * 2 + 1, optimizedWeight);
    shaderStr += Util::str_format(
        "gl_FragColor += texture2D(inputImageTexture, "
        "blurCoordinates[%d]) * %f;\n",
        i * 2 + 2, optimizedWeight);
  }

  // If the number of required samples exceeds the amount we can pass in via
  // varyings, we have to do dependent texture reads in the fragment shader
  if (trueNumberOfOptimizedOffsets > numberOfOptimizedOffsets) {
#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_ANDROID)
    shaderStr += Util::str_format(
        "highp vec2 texelSpacing = vec2(texelWidthOffset, "
        "texelHeightOffset);\n");
#elif defined(GPUPIXEL_MAC) || defined(GPUPIXEL_WIN) || defined(GPUPIXEL_LINUX)
    shaderStr += Util::str_format(
        "vec2 texelSpacing = vec2(texelWidthOffset, texelHeightOffset);\n");
#endif
    for (int i = numberOfOptimizedOffsets; i < trueNumberOfOptimizedOffsets;
         i++) {
      float firstWeight = standardGaussianWeights[i * 2 + 1];
      float secondWeight = standardGaussianWeights[i * 2 + 2];

      float optimizedWeight = firstWeight + secondWeight;
      float optimizedOffset =
          (firstWeight * (i * 2 + 1) + secondWeight * (i * 2 + 2)) /
          optimizedWeight;

      shaderStr += Util::str_format(
          "gl_FragColor += texture2D(inputImageTexture, "
          "blurCoordinates[0] + texelSpacing * %f) * %f;\n",
          optimizedOffset, optimizedWeight);

      shaderStr += Util::str_format(
          "gl_FragColor += texture2D(inputImageTexture, "
          "blurCoordinates[0] - texelSpacing * %f) * %f;\n",
          optimizedOffset, optimizedWeight);
    }
  }

  shaderStr += "}";

  delete[] standardGaussianWeights;
  standardGaussianWeights = 0;
  return shaderStr;
}

}  // namespace gpupixel
