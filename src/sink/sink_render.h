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

  void init();
  virtual void setInputFramebuffer(std::shared_ptr<GPUPixelFramebuffer> framebuffer,
                                   RotationMode rotationMode = NoRotation,
                                   int texIdx = 0) override;
  void setFillMode(FillMode fillMode);
  void setMirror(bool mirror);
  void onSizeChanged(int width, int height);
  virtual void render() override;

 private:
  int _viewWidth;
  int _viewHeight;
  FillMode _fillMode;
  bool _mirror = false;
  GPUPixelGLProgram* _displayProgram;
  GLuint _positionAttribLocation;
  GLuint _texCoordAttribLocation;
  GLuint _colorMapUniformLocation;
  struct {
    float r;
    float g;
    float b;
    float a;
  } _backgroundColor;

  GLfloat _displayVertices[8];

  void _updateDisplayVertices();
  const GLfloat* _getTexureCoordinate(RotationMode rotationMode);
};

}
