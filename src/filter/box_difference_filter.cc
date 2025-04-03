/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "box_difference_filter.h"
#include "gpupixel_context.h"

namespace gpupixel {

const std::string kBoxDifferenceVertexShaderString = R"(
    attribute vec4 position; attribute vec4 inputTextureCoordinate;
    attribute vec4 inputTextureCoordinate2;

    varying vec2 textureCoordinate;
    varying vec2 textureCoordinate2;

    void main() {
      gl_Position = position;
      textureCoordinate = inputTextureCoordinate.xy;
      textureCoordinate2 = inputTextureCoordinate2.xy;
    })";
#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_ANDROID)
const std::string kBoxDifferenceFragmentShaderString = R"(
    varying highp vec2 textureCoordinate; varying highp vec2 textureCoordinate2;
    uniform sampler2D inputImageTexture;
    uniform sampler2D inputImageTexture2;
    uniform highp float delta;

    void main() {
      lowp vec3 iColor = texture2D(inputImageTexture, textureCoordinate).rgb;
      lowp vec3 meanColor =
          texture2D(inputImageTexture2, textureCoordinate2).rgb;
      highp vec3 diffColor = (iColor - meanColor) * delta;
      diffColor = min(diffColor * diffColor, 1.0);
      gl_FragColor = vec4(diffColor, 1.0);
    })";
#elif defined(GPUPIXEL_MAC) || defined(GPUPIXEL_WIN) || defined(GPUPIXEL_LINUX)
const std::string kBoxDifferenceFragmentShaderString = R"(
    varying vec2 textureCoordinate; varying vec2 textureCoordinate2;
    uniform sampler2D inputImageTexture;
    uniform sampler2D inputImageTexture2;
    uniform float delta;

    void main() {
      vec3 iColor = texture2D(inputImageTexture, textureCoordinate).rgb;
      vec3 meanColor = texture2D(inputImageTexture2, textureCoordinate2).rgb;
      vec3 diffColor = (iColor - meanColor) * delta;
      diffColor = min(diffColor * diffColor, 1.0);
      gl_FragColor = vec4(diffColor, 1.0);
    })";
#endif

BoxDifferenceFilter::BoxDifferenceFilter() {}

BoxDifferenceFilter::~BoxDifferenceFilter() {}

std::shared_ptr<BoxDifferenceFilter> BoxDifferenceFilter::Create() {
  auto ret = std::shared_ptr<BoxDifferenceFilter>(new BoxDifferenceFilter());
  if (ret && !ret->Init()) {
    ret.reset();
  }
  return ret;
}

bool BoxDifferenceFilter::Init() {
  if (!Filter::InitWithShaderString(kBoxDifferenceVertexShaderString,
                                    kBoxDifferenceFragmentShaderString, 2)) {
    return false;
  }

  // texcoord attribute
  filterTexCoordAttribute_ =
      _filterProgram->GetAttribLocation("inputTextureCoordinate");
  filterTexCoordAttribute2_ =
      _filterProgram->GetAttribLocation("inputTextureCoordinate2");

  setDelta(7.07);
  return true;
}

bool BoxDifferenceFilter::DoRender(bool updateSinks) {
  static const GLfloat imageVertices[] = {
      -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f,
  };

  GPUPixelContext::GetInstance()->SetActiveGlProgram(_filterProgram);
  _framebuffer->Active();
  CHECK_GL(glClearColor(background_color_.r, background_color_.g,
                        background_color_.b, background_color_.a));
  CHECK_GL(glClear(GL_COLOR_BUFFER_BIT));

  // Texture 0
  CHECK_GL(glActiveTexture(GL_TEXTURE0));
  CHECK_GL(glBindTexture(GL_TEXTURE_2D,
                         input_framebuffers_[0].frameBuffer->GetTexture()));
  _filterProgram->SetUniformValue("inputImageTexture", 0);

  // Texture 1
  CHECK_GL(glActiveTexture(GL_TEXTURE1));
  CHECK_GL(glBindTexture(GL_TEXTURE_2D,
                         input_framebuffers_[1].frameBuffer->GetTexture()));
  _filterProgram->SetUniformValue("inputImageTexture2", 1);

  CHECK_GL(glEnableVertexAttribArray(filterTexCoordAttribute_));
  CHECK_GL(glVertexAttribPointer(
      filterTexCoordAttribute_, 2, GL_FLOAT, 0, 0,
      GetTexureCoordinate(input_framebuffers_[0].rotationMode)));

  CHECK_GL(glEnableVertexAttribArray(filterTexCoordAttribute2_));
  CHECK_GL(glVertexAttribPointer(
      filterTexCoordAttribute2_, 2, GL_FLOAT, 0, 0,
      GetTexureCoordinate(input_framebuffers_[1].rotationMode)));

  // vertex position
  CHECK_GL(glVertexAttribPointer(_filterPositionAttribute, 2, GL_FLOAT, 0, 0,
                                 imageVertices));

  // update uniform
  _filterProgram->SetUniformValue("delta", delta_);

  // draw
  CHECK_GL(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));

  _framebuffer->Inactive();

  return Source::DoRender(updateSinks);
}

void BoxDifferenceFilter::setDelta(float delta) {
  this->delta_ = delta;
}
}
