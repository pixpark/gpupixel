/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "gpupixel/filter/box_difference_filter.h"
#include "core/gpupixel_context.h"
#include "utils/util.h"
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
#if defined(GPUPIXEL_GLES_SHADER)
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
#elif defined(GPUPIXEL_GL_SHADER)
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
  gpupixel::GPUPixelContext::GetInstance()->SyncRunWithContext([&] {
    if (ret && !ret->Init()) {
      ret.reset();
    }
  });
  return ret;
}

bool BoxDifferenceFilter::Init() {
  if (!Filter::InitWithShaderString(kBoxDifferenceVertexShaderString,
                                    kBoxDifferenceFragmentShaderString, 2)) {
    return false;
  }

  // texcoord attribute
  filter_texture_coordinate_attribute_ =
      filter_program_->GetAttribLocation("inputTextureCoordinate");
  filter_texture_coordinate_attribute2_ =
      filter_program_->GetAttribLocation("inputTextureCoordinate2");

  SetDelta(7.07);
  return true;
}

bool BoxDifferenceFilter::DoRender(bool updateSinks) {
  static const float imageVertices[] = {
      -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f,
  };

  GPUPixelContext::GetInstance()->SetActiveGlProgram(filter_program_);
  framebuffer_->Activate();
  GL_CALL(glClearColor(background_color_.r, background_color_.g,
                       background_color_.b, background_color_.a));
  GL_CALL(glClear(GL_COLOR_BUFFER_BIT));

  // Texture 0
  GL_CALL(glActiveTexture(GL_TEXTURE0));
  GL_CALL(glBindTexture(GL_TEXTURE_2D,
                        input_framebuffers_[0].frame_buffer->GetTexture()));
  filter_program_->SetUniformValue("inputImageTexture", 0);

  // Texture 1
  GL_CALL(glActiveTexture(GL_TEXTURE1));
  GL_CALL(glBindTexture(GL_TEXTURE_2D,
                        input_framebuffers_[1].frame_buffer->GetTexture()));
  filter_program_->SetUniformValue("inputImageTexture2", 1);

  GL_CALL(glEnableVertexAttribArray(filter_texture_coordinate_attribute_));
  GL_CALL(glVertexAttribPointer(
      filter_texture_coordinate_attribute_, 2, GL_FLOAT, 0, 0,
      GetTextureCoordinate(input_framebuffers_[0].rotation_mode)));

  GL_CALL(glEnableVertexAttribArray(filter_texture_coordinate_attribute2_));
  GL_CALL(glVertexAttribPointer(
      filter_texture_coordinate_attribute2_, 2, GL_FLOAT, 0, 0,
      GetTextureCoordinate(input_framebuffers_[1].rotation_mode)));

  // vertex position
  GL_CALL(glVertexAttribPointer(filter_position_attribute_, 2, GL_FLOAT, 0, 0,
                                imageVertices));

  // update uniform
  filter_program_->SetUniformValue("delta", delta_);

  // draw
  GL_CALL(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));

  framebuffer_->Deactivate();

  return Source::DoRender(updateSinks);
}

void BoxDifferenceFilter::SetDelta(float delta) {
  this->delta_ = delta;
}
}  // namespace gpupixel
