/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "filter.h"

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
  static std::shared_ptr<FaceMakeupFilter> create();
  ~FaceMakeupFilter();
  virtual bool init();
  virtual bool doRender(bool updateSinks = true) override;

 
  inline void setBlendLevel(float level) { this->blend_level_ = level; }
  void SetFaceLandmarks(std::vector<float> landmarks);
 protected:
  FaceMakeupFilter();
  void setImageTexture(std::shared_ptr<SourceImage> texture);
  void setTextureBounds(FrameBounds bounds) { texture_bounds_ = bounds; }

 private:
  std::vector<GLuint> getFaceIndexs();
  std::vector<GLfloat> faceTextureCoordinates();

 private:
  std::vector<float> face_land_marks_;
  float blend_level_ = 0;  //[0. 0.5]
  bool has_face_ = false;
  //
  GPUPixelGLProgram* _filterProgram2;
  GLuint _filterPositionAttribute2;
  GLuint _filterTexCoordAttribute = 0;
  GLuint _filterTexCoordAttribute2 = 0;

  FrameBounds texture_bounds_;
  std::shared_ptr<SourceImage> image_texture_;
};

}
