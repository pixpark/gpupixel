/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "gpupixel_program.h"
#include "sink.h"

namespace gpupixel {
class GPUPIXEL_API SinkRender : public Sink {
 public:
  enum FillMode {
    Stretch = 0,  // Stretch to fill the view, and may distort the image
    PreserveAspectRatio = 1,  // preserve the aspect ratio of the image
    PreserveAspectRatioAndFill =
        2  // preserve the aspect ratio, and zoom in to fill the view
  };

 public:
  SinkRender();
  ~SinkRender();

  void Init();
  virtual void SetInputFramebuffer(std::shared_ptr<GPUPixelFramebuffer> framebuffer,
                                   RotationMode rotationMode = NoRotation,
                                   int texIdx = 0) override;
  void SetFillMode(FillMode fillMode);
  void SetMirror(bool mirror);
  void SetRenderSize(int width, int height);
  virtual void Render() override;

 private:
  int _viewWidth;
  int _viewHeight;
  FillMode _fillMode;
  bool _mirror = false;
  GPUPixelGLProgram* display_glprogram_;
  GLuint position_attribute_location_;
  GLuint _texCoordAttribLocation;
  GLuint _colorMapUniformLocation;
  struct {
    float r;
    float g;
    float b;
    float a;
  } background_color_;

  GLfloat display_vertices_[8];

  void UpdateDisplayVertices();
  const GLfloat* GetTexureCoordinate(RotationMode rotationMode);
};

}
