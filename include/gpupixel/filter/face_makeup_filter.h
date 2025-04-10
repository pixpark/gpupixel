/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "gpupixel/filter/filter.h"

namespace gpupixel {
class SourceImage;

GPUPIXEL_API typedef struct {
  float x;
  float y;
  float width;
  float height;
} FrameBounds;

class GPUPIXEL_API FaceMakeupFilter : public Filter {
 public:
  static std::shared_ptr<FaceMakeupFilter> Create();
  ~FaceMakeupFilter();
  virtual bool Init();
  virtual bool DoRender(bool updateSinks = true) override;

  inline void SetBlendLevel(float level) { this->blend_level_ = level; }
  void SetFaceLandmarks(std::vector<float> landmarks);

 protected:
  FaceMakeupFilter();
  void SetImageTexture(std::shared_ptr<SourceImage> texture);
  void SetTextureBounds(FrameBounds bounds) { texture_bounds_ = bounds; }

 private:
  std::vector<uint32_t> GetFaceIndexs();
  std::vector<float> FaceTextureCoordinates();

 private:
  std::vector<float> face_landmarks_;
  float blend_level_ = 0;  //[0. 0.5]
  bool has_face_ = false;
  //
  GPUPixelGLProgram* filter_program2_ = nullptr;
  uint32_t filter_position_attribute2_ = 0;
  uint32_t filter_tex_coord_attribute_ = 0;
  uint32_t filter_tex_coord_attribute2_ = 0;

  FrameBounds texture_bounds_;
  std::shared_ptr<SourceImage> image_texture_;
};

}  // namespace gpupixel
