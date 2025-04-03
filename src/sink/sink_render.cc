/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "sink_render.h"
#include "gpupixel_context.h"
#include "util.h"
#include "filter.h"

namespace gpupixel {

SinkRender::SinkRender()
    : _viewWidth(0),
      _viewHeight(0),
      _fillMode(FillMode::PreserveAspectRatio),
      display_glprogram_(0),
      position_attribute_location_(0),
      _texCoordAttribLocation(0),
      _colorMapUniformLocation(0) {
  background_color_.r = 0.0;
  background_color_.g = 0.0;
  background_color_.b = 0.0;
  background_color_.a = 0.0;
  Init();
}

SinkRender::~SinkRender() {
  if (display_glprogram_) {
    delete display_glprogram_;
    display_glprogram_ = 0;
  }
}

void SinkRender::Init() {
  display_glprogram_ = GPUPixelGLProgram::createByShaderString(kDefaultVertexShader,
                                                    kDefaultFragmentShader);
  position_attribute_location_ = display_glprogram_->GetAttribLocation("position");
  _texCoordAttribLocation =
      display_glprogram_->GetAttribLocation("inputTextureCoordinate");
  _colorMapUniformLocation =
      display_glprogram_->GetUniformLocation("textureCoordinate");
  GPUPixelContext::GetInstance()->SetActiveGlProgram(display_glprogram_);
  CHECK_GL(glEnableVertexAttribArray(position_attribute_location_));
  CHECK_GL(glEnableVertexAttribArray(_texCoordAttribLocation));
};

void SinkRender::SetInputFramebuffer(
    std::shared_ptr<GPUPixelFramebuffer> framebuffer,
    RotationMode rotationMode /* = NoRotation*/,
    int texIdx /* = 0*/) {
  std::shared_ptr<GPUPixelFramebuffer> lastInputFramebuffer;
  RotationMode lastInputRotation = NoRotation;
  if (input_framebuffers_.find(0) != input_framebuffers_.end()) {
    lastInputFramebuffer = input_framebuffers_[0].frameBuffer;
    lastInputRotation = input_framebuffers_[0].rotationMode;
  }

  Sink::SetInputFramebuffer(framebuffer, rotationMode, texIdx);

  if (lastInputFramebuffer != framebuffer && framebuffer &&
      (!lastInputFramebuffer ||
       !(lastInputFramebuffer->GetWidth() == framebuffer->GetWidth() &&
         lastInputFramebuffer->GetHeight() == framebuffer->GetHeight() &&
         lastInputRotation == rotationMode))) {
    UpdateDisplayVertices();
  }
}

void SinkRender::SetFillMode(FillMode fillMode) {
  if (_fillMode != fillMode) {
    _fillMode = fillMode;
    UpdateDisplayVertices();
  }
}

void SinkRender::SetMirror(bool mirror) {
  if (_mirror != mirror) {
    _mirror = mirror;
  }
}


void SinkRender::SetRenderSize(int width, int height) {
  if (_viewWidth != width || _viewHeight != height) {
    _viewWidth = width;
    _viewHeight = height;
    UpdateDisplayVertices();
  }
}

void SinkRender::Render() {
  CHECK_GL(glBindFramebuffer(GL_FRAMEBUFFER, 0));

  CHECK_GL(glViewport(0, 0, _viewWidth, _viewHeight));
  CHECK_GL(glClearColor(background_color_.r, background_color_.g,
                        background_color_.b, background_color_.a));
  CHECK_GL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
  GPUPixelContext::GetInstance()->SetActiveGlProgram(display_glprogram_);
  CHECK_GL(glActiveTexture(GL_TEXTURE0));
  CHECK_GL(glBindTexture(GL_TEXTURE_2D,
                         input_framebuffers_[0].frameBuffer->GetTexture()));
  CHECK_GL(glUniform1i(_colorMapUniformLocation, 0));
  CHECK_GL(glVertexAttribPointer(position_attribute_location_, 2, GL_FLOAT, 0, 0,
                                 display_vertices_));
  CHECK_GL(glVertexAttribPointer(
      _texCoordAttribLocation, 2, GL_FLOAT, 0, 0,
      GetTexureCoordinate(input_framebuffers_[0].rotationMode)));
  CHECK_GL(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));
}

void SinkRender::UpdateDisplayVertices() {
  if (input_framebuffers_.find(0) == input_framebuffers_.end() ||
      input_framebuffers_[0].frameBuffer == 0) {
    return;
  }

  std::shared_ptr<GPUPixelFramebuffer> inputFramebuffer =
      input_framebuffers_[0].frameBuffer;
  RotationMode inputRotation = input_framebuffers_[0].rotationMode;

  int rotatedFramebufferWidth = inputFramebuffer->GetWidth();
  int rotatedFramebufferHeight = inputFramebuffer->GetHeight();
  if (rotationSwapsSize(inputRotation)) {
    rotatedFramebufferWidth = inputFramebuffer->GetHeight();
    rotatedFramebufferHeight = inputFramebuffer->GetWidth();
  }

  float framebufferAspectRatio =
      rotatedFramebufferHeight / (float)rotatedFramebufferWidth;
  float viewAspectRatio = _viewHeight / (float)_viewWidth;

  float insetFramebufferWidth = 0.0;
  float insetFramebufferHeight = 0.0;
  if (framebufferAspectRatio > viewAspectRatio) {
    insetFramebufferWidth =
        _viewHeight / (float)rotatedFramebufferHeight * rotatedFramebufferWidth;
    insetFramebufferHeight = _viewHeight;
  } else {
    insetFramebufferWidth = _viewWidth;
    insetFramebufferHeight =
        _viewWidth / (float)rotatedFramebufferWidth * rotatedFramebufferHeight;
  }

  float scaledWidth = 1.0;
  float scaledHeight = 1.0;
  if (_fillMode == FillMode::PreserveAspectRatio) {
    scaledWidth = insetFramebufferWidth / _viewWidth;
    scaledHeight = insetFramebufferHeight / _viewHeight;
  } else if (_fillMode == FillMode::PreserveAspectRatioAndFill) {
    scaledWidth = _viewWidth / insetFramebufferHeight;
    scaledHeight = _viewHeight / insetFramebufferWidth;
  }

  display_vertices_[0] = -scaledWidth;
  display_vertices_[1] = -scaledHeight;
  display_vertices_[2] = scaledWidth;
  display_vertices_[3] = -scaledHeight;
  display_vertices_[4] = -scaledWidth;
  display_vertices_[5] = scaledHeight;
  display_vertices_[6] = scaledWidth;
  display_vertices_[7] = scaledHeight;
}

const GLfloat* SinkRender::GetTexureCoordinate(RotationMode rotationMode) {
  static const GLfloat noRotationTextureCoordinates[] = {
    0.0f, 1.0f,
    1.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 0.0f,
  };

  static const GLfloat rotateRightTextureCoordinates[] = {
    1.0f, 1.0f,
    1.0f, 0.0f,
    0.0f, 1.0f,
    0.0f, 0.0f,
  };

  static const GLfloat rotateLeftTextureCoordinates[] = {
    0.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
  };

  static const GLfloat verticalFlipTextureCoordinates[] = {
    0.0f, 0.0f,
    1.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 1.0f,
  };

  static const GLfloat horizontalFlipTextureCoordinates[] = {
    1.0f, 1.0f,
    0.0f, 1.0f,
    1.0f, 0.0f,
    0.0f, 0.0f,
  };

  static const GLfloat rotateRightVerticalFlipTextureCoordinates[] = {
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 0.0f,
    0.0f, 1.0f,
  };

  static const GLfloat rotateRightHorizontalFlipTextureCoordinates[] = {
    0.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 1.0f,
    1.0f, 0.0f,
  };

  static const GLfloat rotate180TextureCoordinates[] = {
    1.0f, 0.0f,
    0.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,
  };

  switch (rotationMode) {
    case NoRotation: {
      if(_mirror) {
        return horizontalFlipTextureCoordinates;
      } else {
        return noRotationTextureCoordinates;
      }
    }
    case RotateLeft:
      return rotateLeftTextureCoordinates;
    case RotateRight:
      return rotateRightTextureCoordinates;
    case FlipVertical:
      return verticalFlipTextureCoordinates;
    case FlipHorizontal:
      return horizontalFlipTextureCoordinates;
    case RotateRightFlipVertical:
      return rotateRightVerticalFlipTextureCoordinates;
    case RotateRightFlipHorizontal:
      return rotateRightHorizontalFlipTextureCoordinates;
    case Rotate180:
      return rotate180TextureCoordinates;
  }
}

} // namespace gpupixel