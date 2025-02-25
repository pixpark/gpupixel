/*
 * GPUPixel
 *
 * Created by pixpark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "background_segmentation_filter.h"

USING_NS_GPUPIXEL

REGISTER_FILTER_CLASS(BackgroundSegmentationFilter)

const std::string kBackgroundSegmentationVertexShaderString = R"(
    attribute vec4 position; 
    attribute vec4 inputTextureCoordinate;
    varying vec2 textureCoordinate;

    void main() {
      textureCoordinate = (inputTextureCoordinate).xy;
      gl_Position = position;
    })";

#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_ANDROID)
const std::string kBackgroundSegmentationFragmentShaderString = R"(
    varying mediump vec2 textureCoordinate; 
    uniform sampler2D inputImageTexture;
    uniform sampler2D maskTexture;
    void main() {
      float gray = texture2D(maskTexture, textureCoordinate).r;
      vec4 rgba = texture2D(inputImageTexture, textureCoordinate);
      if(gray > 0.01) {
          gl_FragColor = rgba;
      } else {
          float Y = 0.299 * rgba.r + 0.587 * rgba.g + 0.114 * rgba.b;
          vec4 grayColor = vec4(vec3(Y), 1.0);
          gl_FragColor = mix(grayColor, rgba, 1.0);
      }
    })";
#elif defined(GPUPIXEL_MAC) || defined(GPUPIXEL_WIN) || defined(GPUPIXEL_LINUX)
const std::string kBackgroundSegmentationFragmentShaderString = R"(
    precision mediump float;
    varying vec2 textureCoordinate; 
    uniform sampler2D inputImageTexture;
    uniform sampler2D maskTexture;
    void main() {
      float gray = texture2D(maskTexture, textureCoordinate).r;
      vec4 rgba = texture2D(inputImageTexture, textureCoordinate);
      if(gray > 0.01) {
          gl_FragColor = rgba;
      } else {
          float Y = 0.299 * rgba.r + 0.587 * rgba.g + 0.114 * rgba.b;
          vec4 grayColor = vec4(vec3(Y), 1.0);
          gl_FragColor = mix(grayColor, rgba, 1.0);
      }
    })";
#endif

std::shared_ptr<BackgroundSegmentationFilter>
BackgroundSegmentationFilter::create() {
  auto ret = std::shared_ptr<BackgroundSegmentationFilter>(
      new BackgroundSegmentationFilter());
  if (ret && !ret->init()) {
    ret.reset();
  }
  return ret;
}

bool BackgroundSegmentationFilter::init() {
  if (!initWithShaderString(kBackgroundSegmentationVertexShaderString,
                            kBackgroundSegmentationFragmentShaderString)) {
    return false;
  }

  // _brightness = 0.01;
  // registerProperty("brightness_para", _brightness,
  //                  "The brightness of filter with range between -1 and 1.",
  //                  [this](float& brightness) { setBrightness(brightness); });

  return true;
}

// void BackgroundSegmentationFilter::setBrightness() {
//   _brightness = brightness;
//   if (_brightness > 1.0) {
//     _brightness = 1.0;
//   } else if (_brightness < -1.0) {
//     _brightness = -1.0;
//   }
// }

bool BackgroundSegmentationFilter::proceed(bool bUpdateTargets,
                                           int64_t frameTime) {
  _filterProgram->setUniformValue("brightness_para", _brightness);
  return Filter::proceed(bUpdateTargets, frameTime);
}
