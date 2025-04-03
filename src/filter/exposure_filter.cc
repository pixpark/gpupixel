/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "exposure_filter.h"

namespace gpupixel {

const std::string kExposureFragmentShaderString = R"(
    uniform sampler2D inputImageTexture; uniform lowp float exposure;
    varying highp vec2 textureCoordinate;

    void main() {
      lowp vec4 color = texture2D(inputImageTexture, textureCoordinate);
      gl_FragColor = vec4(color.rgb * pow(2.0, exposure), color.a);
    })";

std::shared_ptr<ExposureFilter> ExposureFilter::Create() {
  auto ret = std::shared_ptr<ExposureFilter>(new ExposureFilter());
  if (ret && !ret->Init()) {
    ret.reset();
  }
  return ret;
}

bool ExposureFilter::Init() {
  if (!InitWithFragmentShaderString(kExposureFragmentShaderString)) {
    return false;
  }

  _exposure = 0.0;
  RegisterProperty("exposure", _exposure,
                   "The exposure of the image. Exposure ranges from -10.0 to "
                   "10.0 (max contrast), with 0.0 as the normal level",
                   [this](float& exposure) { SetExposure(exposure); });

  return true;
}

void ExposureFilter::SetExposure(float exposure) {
  _exposure = exposure;
  if (_exposure > 10.0) {
    _exposure = 10.0;
  } else if (_exposure < -10.0) {
    _exposure = -10.0;
  }
}

bool ExposureFilter::DoRender(bool updateSinks) {
  _filterProgram->SetUniformValue("exposure", _exposure);
  return Filter::DoRender(updateSinks);
}

} // namespace gpupixel