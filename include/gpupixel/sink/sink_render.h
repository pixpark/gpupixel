/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "gpupixel/sink/sink.h"

namespace gpupixel {
class GPUPixelGLProgram;
class GPUPIXEL_API SinkRender : public Sink {
 public:
  enum FillMode {
    Stretch = 0,  // Stretch to fill the view, and may distort the image
    PreserveAspectRatio = 1,  // preserve the aspect ratio of the image
    PreserveAspectRatioAndFill =
        2  // preserve the aspect ratio, and zoom in to fill the view
  };
  std::shared_ptr<SinkRender> Create();

  ~SinkRender() override;

  void Init();
  void SetInputFramebuffer(std::shared_ptr<GPUPixelFramebuffer> framebuffer,
                           RotationMode rotation_mode = NoRotation,
                           int tex_idx = 0) override;
  void SetFillMode(FillMode fill_mode);
  void SetMirror(bool mirror);
  void SetRenderSize(int width, int height);
  void Render() override;

 private:
  SinkRender();
  int view_width_;
  int view_height_;
  FillMode fill_mode_;
  bool mirror_ = false;
  GPUPixelGLProgram* display_program_;
  uint32_t position_attribute_location_;
  uint32_t tex_coord_attribute_location_;
  uint32_t color_map_uniform_location_;
  struct {
    float r;
    float g;
    float b;
    float a;
  } background_color_;

  float display_vertices_[8];

  void UpdateDisplayVertices();
  const float* GetTextureCoordinate(RotationMode rotation_mode);
};

}  // namespace gpupixel
