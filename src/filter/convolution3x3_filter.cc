/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "gpupixel/filter/convolution3x3_filter.h"
#include <glm/glm.hpp>
#include "core/gpupixel_context.h"

namespace gpupixel {

#if defined(GPUPIXEL_GLES_SHADER)
const std::string kConvolution3x3FragmentShaderString = R"(
    precision highp float; uniform sampler2D inputImageTexture;
    uniform mediump mat3 convolutionMatrix;

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
      mediump vec3 bottomColor =
          texture2D(inputImageTexture, vBottomTexCoord).rgb;
      mediump vec3 bottomLeftColor =
          texture2D(inputImageTexture, vBottomLeftTexCoord).rgb;
      mediump vec3 bottomRightColor =
          texture2D(inputImageTexture, vBottomRightTexCoord).rgb;
      mediump vec4 centerColor =
          texture2D(inputImageTexture, textureCoordinate);
      mediump vec3 leftColor = texture2D(inputImageTexture, vLeftTexCoord).rgb;
      mediump vec3 rightColor =
          texture2D(inputImageTexture, vRightTexCoord).rgb;
      mediump vec3 topColor = texture2D(inputImageTexture, vTopTexCoord).rgb;
      mediump vec3 topRightColor =
          texture2D(inputImageTexture, vTopRightTexCoord).rgb;
      mediump vec3 topLeftColor =
          texture2D(inputImageTexture, vTopLeftTexCoord).rgb;

      mediump vec3 resultColor = topLeftColor * convolutionMatrix[0][0] +
                                 topColor * convolutionMatrix[0][1] +
                                 topRightColor * convolutionMatrix[0][2];
      resultColor += leftColor * convolutionMatrix[1][0] +
                     centerColor.rgb * convolutionMatrix[1][1] +
                     rightColor * convolutionMatrix[1][2];
      resultColor += bottomLeftColor * convolutionMatrix[2][0] +
                     bottomColor * convolutionMatrix[2][1] +
                     bottomRightColor * convolutionMatrix[2][2];

      gl_FragColor = vec4(resultColor, centerColor.a);
    })";
#elif defined(GPUPIXEL_GL_SHADER)
const std::string kConvolution3x3FragmentShaderString = R"(
    precision float; uniform sampler2D inputImageTexture;
    uniform mat3 convolutionMatrix;

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
      vec3 bottomColor = texture2D(inputImageTexture, vBottomTexCoord).rgb;
      vec3 bottomLeftColor =
          texture2D(inputImageTexture, vBottomLeftTexCoord).rgb;
      vec3 bottomRightColor =
          texture2D(inputImageTexture, vBottomRightTexCoord).rgb;
      vec4 centerColor = texture2D(inputImageTexture, textureCoordinate);
      vec3 leftColor = texture2D(inputImageTexture, vLeftTexCoord).rgb;
      vec3 rightColor = texture2D(inputImageTexture, vRightTexCoord).rgb;
      vec3 topColor = texture2D(inputImageTexture, vTopTexCoord).rgb;
      vec3 topRightColor = texture2D(inputImageTexture, vTopRightTexCoord).rgb;
      vec3 topLeftColor = texture2D(inputImageTexture, vTopLeftTexCoord).rgb;

      vec3 resultColor = topLeftColor * convolutionMatrix[0][0] +
                         topColor * convolutionMatrix[0][1] +
                         topRightColor * convolutionMatrix[0][2];
      resultColor += leftColor * convolutionMatrix[1][0] +
                     centerColor.rgb * convolutionMatrix[1][1] +
                     rightColor * convolutionMatrix[1][2];
      resultColor += bottomLeftColor * convolutionMatrix[2][0] +
                     bottomColor * convolutionMatrix[2][1] +
                     bottomRightColor * convolutionMatrix[2][2];

      gl_FragColor = vec4(resultColor, centerColor.a);
    })";
#endif

bool Convolution3x3Filter::Init() {
  if (!NearbySampling3x3Filter::InitWithFragmentShaderString(
          kConvolution3x3FragmentShaderString)) {
    return false;
  }

  // Initialize with identity matrix
  convolution_kernel_ = glm::mat3(0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f);

  return true;
}

bool Convolution3x3Filter::DoRender(bool updateSinks) {
  filter_program_->SetUniformValue("convolutionMatrix", convolution_kernel_);
  return NearbySampling3x3Filter::DoRender(updateSinks);
}

}  // namespace gpupixel
