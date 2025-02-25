/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "crosshatch_filter.h"

USING_NS_GPUPIXEL

#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_ANDROID)
const std::string kCrosshatchFragmentShaderString = R"(
    uniform sampler2D inputImageTexture; varying highp vec2 textureCoordinate;
    uniform highp float crossHatchSpacing;
    uniform highp float lineWidth;

    const highp vec3 W = vec3(0.2125, 0.7154, 0.0721);

    void main() {
      highp float luminance =
          dot(texture2D(inputImageTexture, textureCoordinate).rgb, W);

      lowp vec4 colorToDisplay = vec4(1.0, 1.0, 1.0, 1.0);
      if (luminance < 1.00) {
        if (mod(textureCoordinate.x + textureCoordinate.y, crossHatchSpacing) <=
            lineWidth) {
          colorToDisplay = vec4(0.0, 0.0, 0.0, 1.0);
        }
      }
      if (luminance < 0.75) {
        if (mod(textureCoordinate.x - textureCoordinate.y, crossHatchSpacing) <=
            lineWidth) {
          colorToDisplay = vec4(0.0, 0.0, 0.0, 1.0);
        }
      }
      if (luminance < 0.50) {
        if (mod(textureCoordinate.x + textureCoordinate.y -
                    (crossHatchSpacing / 2.0),
                crossHatchSpacing) <= lineWidth) {
          colorToDisplay = vec4(0.0, 0.0, 0.0, 1.0);
        }
      }
      if (luminance < 0.3) {
        if (mod(textureCoordinate.x - textureCoordinate.y -
                    (crossHatchSpacing / 2.0),
                crossHatchSpacing) <= lineWidth) {
          colorToDisplay = vec4(0.0, 0.0, 0.0, 1.0);
        }
      }

      gl_FragColor = colorToDisplay;
    })";
#elif defined(GPUPIXEL_MAC) || defined(GPUPIXEL_WIN) || defined(GPUPIXEL_LINUX)
const std::string kCrosshatchFragmentShaderString = R"(
    precision mediump float;
    uniform sampler2D inputImageTexture; varying vec2 textureCoordinate;
    uniform float crossHatchSpacing;
    uniform float lineWidth;

    const vec3 W = vec3(0.2125, 0.7154, 0.0721);

    void main() {
      float luminance =
          dot(texture2D(inputImageTexture, textureCoordinate).rgb, W);

      vec4 colorToDisplay = vec4(1.0, 1.0, 1.0, 1.0);
      if (luminance < 1.00) {
        if (mod(textureCoordinate.x + textureCoordinate.y, crossHatchSpacing) <=
            lineWidth) {
          colorToDisplay = vec4(0.0, 0.0, 0.0, 1.0);
        }
      }
      if (luminance < 0.75) {
        if (mod(textureCoordinate.x - textureCoordinate.y, crossHatchSpacing) <=
            lineWidth) {
          colorToDisplay = vec4(0.0, 0.0, 0.0, 1.0);
        }
      }
      if (luminance < 0.50) {
        if (mod(textureCoordinate.x + textureCoordinate.y -
                    (crossHatchSpacing / 2.0),
                crossHatchSpacing) <= lineWidth) {
          colorToDisplay = vec4(0.0, 0.0, 0.0, 1.0);
        }
      }
      if (luminance < 0.3) {
        if (mod(textureCoordinate.x - textureCoordinate.y -
                    (crossHatchSpacing / 2.0),
                crossHatchSpacing) <= lineWidth) {
          colorToDisplay = vec4(0.0, 0.0, 0.0, 1.0);
        }
      }

      gl_FragColor = colorToDisplay;
    })";
#endif

std::shared_ptr<CrosshatchFilter> CrosshatchFilter::create() {
  auto ret = std::shared_ptr<CrosshatchFilter>(new CrosshatchFilter());
  if (ret && !ret->init()) {
    ret.reset();
  }
  return ret;
}

bool CrosshatchFilter::init() {
  if (!initWithFragmentShaderString(kCrosshatchFragmentShaderString)) {
    return false;
  }

  setCrossHatchSpacing(0.03);
  registerProperty("crossHatchSpacing", _crossHatchSpacing,
                   "The fractional width of the image to use as the spacing "
                   "for the crosshatch. The default is 0.03.",
                   [this](float& crossHatchSpacing) {
                     setCrossHatchSpacing(crossHatchSpacing);
                   });

  setLineWidth(0.003);
  registerProperty(
      "lineWidth", _lineWidth,
      "A relative width for the crosshatch lines. The default is 0.003.",
      [this](float& lineWidth) { setLineWidth(lineWidth); });
  return true;
}

bool CrosshatchFilter::proceed(bool bUpdateTargets, int64_t frameTime) {
  _filterProgram->setUniformValue("crossHatchSpacing", _crossHatchSpacing);
  _filterProgram->setUniformValue("lineWidth", _lineWidth);
  return Filter::proceed(bUpdateTargets, frameTime);
}

void CrosshatchFilter::setCrossHatchSpacing(float crossHatchSpacing) {
  _crossHatchSpacing = crossHatchSpacing;
}

void CrosshatchFilter::setLineWidth(float lineWidth) {
  _lineWidth = lineWidth;
}
