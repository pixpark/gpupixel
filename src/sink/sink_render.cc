/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "gpupixel/sink/sink_render.h"
#include "core/gpupixel_context.h"
#include "gpupixel/filter/filter.h"
#include "utils/util.h"

namespace gpupixel {

std::shared_ptr<SinkRender> SinkRender::Create() {
  auto ret = std::shared_ptr<SinkRender>(new SinkRender());
  gpupixel::GPUPixelContext::GetInstance()->SyncRunWithContext(
      [&] { ret->Init(); });
  return ret;
}

SinkRender::SinkRender()
    : view_width_(0),
      view_height_(0),
      fill_mode_(FillMode::PreserveAspectRatio),
      display_program_(0),
      position_attribute_location_(0),
      tex_coord_attribute_location_(0),
      color_map_uniform_location_(0) {
  background_color_.r = 0.0;
  background_color_.g = 0.0;
  background_color_.b = 0.0;
  background_color_.a = 0.0;
}

SinkRender::~SinkRender() {
  if (display_program_) {
    delete display_program_;
    display_program_ = 0;
  }
}

void SinkRender::Init() {
  display_program_ = GPUPixelGLProgram::CreateWithShaderString(
      kDefaultVertexShader, kDefaultFragmentShader);
  position_attribute_location_ =
      display_program_->GetAttribLocation("position");
  tex_coord_attribute_location_ =
      display_program_->GetAttribLocation("inputTextureCoordinate");
  color_map_uniform_location_ =
      display_program_->GetUniformLocation("textureCoordinate");
  GPUPixelContext::GetInstance()->SetActiveGlProgram(display_program_);
  CHECK_GL(glEnableVertexAttribArray(position_attribute_location_));
  CHECK_GL(glEnableVertexAttribArray(tex_coord_attribute_location_));
};

void SinkRender::SetInputFramebuffer(
    std::shared_ptr<GPUPixelFramebuffer> framebuffer,
    RotationMode rotation_mode /* = NoRotation*/,
    int tex_idx /* = 0*/) {
  std::shared_ptr<GPUPixelFramebuffer> last_input_framebuffer;
  RotationMode last_input_rotation = NoRotation;
  if (input_framebuffers_.find(0) != input_framebuffers_.end()) {
    last_input_framebuffer = input_framebuffers_[0].frame_buffer;
    last_input_rotation = input_framebuffers_[0].rotation_mode;
  }

  Sink::SetInputFramebuffer(framebuffer, rotation_mode, tex_idx);

  if (last_input_framebuffer != framebuffer && framebuffer &&
      (!last_input_framebuffer ||
       !(last_input_framebuffer->GetWidth() == framebuffer->GetWidth() &&
         last_input_framebuffer->GetHeight() == framebuffer->GetHeight() &&
         last_input_rotation == rotation_mode))) {
    UpdateDisplayVertices();
  }
}

void SinkRender::SetFillMode(FillMode fill_mode) {
  if (fill_mode_ != fill_mode) {
    fill_mode_ = fill_mode;
    UpdateDisplayVertices();
  }
}

void SinkRender::SetMirror(bool mirror) {
  if (mirror_ != mirror) {
    mirror_ = mirror;
  }
}

void SinkRender::SetRenderSize(int width, int height) {
  if (view_width_ != width || view_height_ != height) {
    view_width_ = width;
    view_height_ = height;
    UpdateDisplayVertices();
  }
}

void SinkRender::Render() {
  CHECK_GL(glBindFramebuffer(GL_FRAMEBUFFER, 0));

  if (view_width_ == 0 || view_height_ == 0) {
    Util::Log("SinkRender", "view_width_ or view_height_ is 0");
    return;
  }
  CHECK_GL(glViewport(0, 0, view_width_, view_height_));
  CHECK_GL(glClearColor(background_color_.r, background_color_.g,
                        background_color_.b, background_color_.a));
  CHECK_GL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
  GPUPixelContext::GetInstance()->SetActiveGlProgram(display_program_);
  CHECK_GL(glActiveTexture(GL_TEXTURE0));
  CHECK_GL(glBindTexture(GL_TEXTURE_2D,
                         input_framebuffers_[0].frame_buffer->GetTexture()));
  CHECK_GL(glUniform1i(color_map_uniform_location_, 0));
  CHECK_GL(glVertexAttribPointer(position_attribute_location_, 2, GL_FLOAT, 0,
                                 0, display_vertices_));
  CHECK_GL(glVertexAttribPointer(
      tex_coord_attribute_location_, 2, GL_FLOAT, 0, 0,
      GetTextureCoordinate(input_framebuffers_[0].rotation_mode)));

  CHECK_GL(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));
}

void SinkRender::UpdateDisplayVertices() {
  if (input_framebuffers_.find(0) == input_framebuffers_.end() ||
      input_framebuffers_[0].frame_buffer == 0) {
    return;
  }

  std::shared_ptr<GPUPixelFramebuffer> input_framebuffer =
      input_framebuffers_[0].frame_buffer;
  RotationMode input_rotation = input_framebuffers_[0].rotation_mode;

  int rotated_framebuffer_width = input_framebuffer->GetWidth();
  int rotated_framebuffer_height = input_framebuffer->GetHeight();
  if (rotationSwapsSize(input_rotation)) {
    rotated_framebuffer_width = input_framebuffer->GetHeight();
    rotated_framebuffer_height = input_framebuffer->GetWidth();
  }

  float framebuffer_aspect_ratio =
      rotated_framebuffer_height / (float)rotated_framebuffer_width;
  float view_aspect_ratio = view_height_ / (float)view_width_;

  float inset_framebuffer_width = 0.0;
  float inset_framebuffer_height = 0.0;
  if (framebuffer_aspect_ratio > view_aspect_ratio) {
    inset_framebuffer_width = view_height_ / (float)rotated_framebuffer_height *
                              rotated_framebuffer_width;
    inset_framebuffer_height = view_height_;
  } else {
    inset_framebuffer_width = view_width_;
    inset_framebuffer_height = view_width_ / (float)rotated_framebuffer_width *
                               rotated_framebuffer_height;
  }

  float scaled_width = 1.0;
  float scaled_height = 1.0;
  if (fill_mode_ == FillMode::PreserveAspectRatio) {
    scaled_width = inset_framebuffer_width / view_width_;
    scaled_height = inset_framebuffer_height / view_height_;
  } else if (fill_mode_ == FillMode::PreserveAspectRatioAndFill) {
    scaled_width = view_width_ / inset_framebuffer_height;
    scaled_height = view_height_ / inset_framebuffer_width;
  }

  display_vertices_[0] = -scaled_width;
  display_vertices_[1] = -scaled_height;
  display_vertices_[2] = scaled_width;
  display_vertices_[3] = -scaled_height;
  display_vertices_[4] = -scaled_width;
  display_vertices_[5] = scaled_height;
  display_vertices_[6] = scaled_width;
  display_vertices_[7] = scaled_height;
}

const float* SinkRender::GetTextureCoordinate(RotationMode rotation_mode) {
  static const float no_rotation_texture_coordinates[] = {
      0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
  };

  static const float rotate_right_texture_coordinates[] = {
      1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
  };

  static const float rotate_left_texture_coordinates[] = {
      0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
  };

  static const float vertical_flip_texture_coordinates[] = {
      0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
  };

  static const float horizontal_flip_texture_coordinates[] = {
      1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
  };

  static const float rotate_right_vertical_flip_texture_coordinates[] = {
      1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
  };

  static const float rotate_right_horizontal_flip_texture_coordinates[] = {
      0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f,
  };

  static const float rotate_180_texture_coordinates[] = {
      1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
  };

  switch (rotation_mode) {
    case NoRotation: {
      if (mirror_) {
        return horizontal_flip_texture_coordinates;
      } else {
        return no_rotation_texture_coordinates;
      }
    }
    case RotateLeft:
      return rotate_left_texture_coordinates;
    case RotateRight:
      return rotate_right_texture_coordinates;
    case FlipVertical:
      return vertical_flip_texture_coordinates;
    case FlipHorizontal:
      return horizontal_flip_texture_coordinates;
    case RotateRightFlipVertical:
      return rotate_right_vertical_flip_texture_coordinates;
    case RotateRightFlipHorizontal:
      return rotate_right_horizontal_flip_texture_coordinates;
    case Rotate180:
      return rotate_180_texture_coordinates;
  }
}

}  // namespace gpupixel
