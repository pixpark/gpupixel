/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "box_mono_blur_filter.h"
#include <cmath>
NS_GPUPIXEL_BEGIN

BoxMonoBlurFilter::BoxMonoBlurFilter(Type type)
    : GaussianBlurMonoFilter(type) {}

BoxMonoBlurFilter::~BoxMonoBlurFilter() {}

std::shared_ptr<BoxMonoBlurFilter> BoxMonoBlurFilter::create(Type type,
                                                             int radius,
                                                             float sigma) {
  auto ret = std::shared_ptr<BoxMonoBlurFilter>(new BoxMonoBlurFilter(type));
  if (ret && !ret->init(radius, sigma)) {
    ret.reset();
  }
  return ret;
}

bool BoxMonoBlurFilter::init(int radius, float sigma) {
  if (Filter::initWithShaderString(
          _generateOptimizedVertexShaderString(radius, sigma),
          _generateOptimizedFragmentShaderString(radius, sigma))) {
    return true;
  }
  return false;
}

void BoxMonoBlurFilter::setRadius(int radius) {
  float newBlurRadius = std::round(std::round(radius / 2.0) *
                                   2.0);  // For now, only do even radii

  if (newBlurRadius != _radius) {
    _radius = newBlurRadius;

    if (_filterProgram) {
      delete _filterProgram;
      _filterProgram = 0;
    }
    initWithShaderString(_generateOptimizedVertexShaderString(_radius, 0.0),
                         _generateOptimizedFragmentShaderString(_radius, 0.0));
  }
}

std::string BoxMonoBlurFilter::_generateOptimizedVertexShaderString(
    int radius,
    float sigma) {
  if (radius < 1) {
    return kDefaultVertexShader;
  }
  // From these weights we calculate the offsets to read interpolated values
  // from
  int numberOfOptimizedOffsets = std::min(radius / 2 + (radius % 2), 7);

  std::string shaderStr =
      // Header
      Util::str_format(
          "\
     attribute vec4 position;\n\
     attribute vec4 inputTextureCoordinate;\n\
     \n\
     uniform float texelWidthOffset;\n\
     uniform float texelHeightOffset;\n\
     \n\
     varying vec2 blurCoordinates[%lu];\n\
     \n\
     void main()\n\
     {\n\
     gl_Position = position;\n\
     \n\
     vec2 singleStepOffset = vec2(texelWidthOffset, texelHeightOffset);\n",
          (unsigned long)(1 + (numberOfOptimizedOffsets * 2)));

  // Inner offset loop
  shaderStr += "blurCoordinates[0] = inputTextureCoordinate.xy;\n";
  for (uint32_t currentOptimizedOffset = 0;
       currentOptimizedOffset < numberOfOptimizedOffsets;
       currentOptimizedOffset++) {
    GLfloat optimizedOffset = (GLfloat)(currentOptimizedOffset * 2) + 1.5;

    shaderStr += Util::str_format(
        "\
         blurCoordinates[%lu] = inputTextureCoordinate.xy + singleStepOffset * %f;\n\
         blurCoordinates[%lu] = inputTextureCoordinate.xy - singleStepOffset * %f;\n",
        (unsigned long)((currentOptimizedOffset * 2) + 1), optimizedOffset,
        (unsigned long)((currentOptimizedOffset * 2) + 2), optimizedOffset);
  }

  // Footer
  shaderStr += "}\n";

  return shaderStr;
}

std::string BoxMonoBlurFilter::_generateOptimizedFragmentShaderString(
    int radius,
    float sigma) {
  if (radius < 1) {
    return kDefaultFragmentShader;
  }

  uint32_t numberOfOptimizedOffsets = std::min(radius / 2 + (radius % 2), 7);
  uint32_t trueNumberOfOptimizedOffsets = radius / 2 + (radius % 2);

  // Header
#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_ANDROID)
  std::string shaderStr =
      // Header
      Util::str_format(
          "\
     uniform sampler2D inputImageTexture;\n\
     uniform highp float texelWidthOffset;\n\
     uniform highp float texelHeightOffset;\n\
     \n\
     varying highp vec2 blurCoordinates[%lu];\n\
     \n\
     void main()\n\
     {\n\
     lowp vec4 sum = vec4(0.0);\n",
          (unsigned long)(1 + (numberOfOptimizedOffsets * 2)));
#else
  std::string shaderStr =
      // Header
      Util::str_format(
          "\
     precision mediump float;\n\
     uniform sampler2D inputImageTexture;\n\
     uniform float texelWidthOffset;\n\
     uniform float texelHeightOffset;\n\
     \n\
     varying vec2 blurCoordinates[%lu];\n\
     \n\
     void main()\n\
     {\n\
     vec4 sum = vec4(0.0);\n",
          1 + (numberOfOptimizedOffsets * 2));
#endif

  GLfloat boxWeight = 1.0 / (GLfloat)((radius * 2) + 1);

  // Inner texture loop
  shaderStr += Util::str_format(
      "sum += texture2D(inputImageTexture, blurCoordinates[0]) * %f;\n",
      boxWeight);

  for (uint32_t currentBlurCoordinateIndex = 0;
       currentBlurCoordinateIndex < numberOfOptimizedOffsets;
       currentBlurCoordinateIndex++) {
    shaderStr += Util::str_format(
        "sum += texture2D(inputImageTexture, blurCoordinates[%lu]) * %f;\n",
        (unsigned long)((currentBlurCoordinateIndex * 2) + 1), boxWeight * 2.0);
    shaderStr += Util::str_format(
        "sum += texture2D(inputImageTexture, blurCoordinates[%lu]) * %f;\n",
        (unsigned long)((currentBlurCoordinateIndex * 2) + 2), boxWeight * 2.0);
  }

  // If the number of required samples exceeds the amount we can pass in via
  // varyings, we have to do dependent texture reads in the fragment shader
  if (trueNumberOfOptimizedOffsets > numberOfOptimizedOffsets) {
#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_ANDROID)
    shaderStr += Util::str_format(
        "highp vec2 singleStepOffset = vec2(texelWidthOffset, "
        "texelHeightOffset);\n");
#else
    shaderStr += Util::str_format(
        "vec2 singleStepOffset = vec2(texelWidthOffset, texelHeightOffset);\n");
#endif

    for (uint32_t currentOverlowTextureRead = numberOfOptimizedOffsets;
         currentOverlowTextureRead < trueNumberOfOptimizedOffsets;
         currentOverlowTextureRead++) {
      GLfloat optimizedOffset = (GLfloat)(currentOverlowTextureRead * 2) + 1.5;

      shaderStr += Util::str_format(
          "sum += texture2D(inputImageTexture, blurCoordinates[0] + "
          "singleStepOffset * %f) * %f;\n",
          optimizedOffset, boxWeight * 2.0);
      shaderStr += Util::str_format(
          "sum += texture2D(inputImageTexture, blurCoordinates[0] - "
          "singleStepOffset * %f) * %f;\n",
          optimizedOffset, boxWeight * 2.0);
    }
  }

  // Footer
  shaderStr +=
      "\
     gl_FragColor = sum;\n\
     }\n";

  return shaderStr;
}

NS_GPUPIXEL_END
