/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "gl_program.h"
#include "target.h"

NS_GPUPIXEL_BEGIN

class TargetView : public Target {
 public:
  enum FillMode {
    Stretch = 0,  // Stretch to fill the view, and may distort the image
    PreserveAspectRatio = 1,  // preserve the aspect ratio of the image
    PreserveAspectRatioAndFill =
        2  // preserve the aspect ratio, and zoom in to fill the view
  };

 public:
  TargetView();
  ~TargetView();

  void init();
  virtual void setInputFramebuffer(std::shared_ptr<Framebuffer> framebuffer,
                                   RotationMode rotationMode = NoRotation,
                                   int texIdx = 0) override;
  void setFillMode(FillMode fillMode);
  void setMirror(bool mirror);
  void onSizeChanged(int width, int height);
  virtual void update(int64_t frameTime) override;

 private:
  int _viewWidth;
  int _viewHeight;
  FillMode _fillMode;
  bool _mirror = false;
  GLProgram* _displayProgram;
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

NS_GPUPIXEL_END
