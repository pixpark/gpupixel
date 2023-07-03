/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "Filter.h"

NS_GPUPIXEL_BEGIN
class SourceImage;

typedef struct {
  float x;
  float y;
  float width;
  float height;
} FrameBounds;

class FaceMakeupFilter : public Filter {
 public:
  static std::shared_ptr<FaceMakeupFilter> create();
  ~FaceMakeupFilter();
  virtual bool init();
  virtual bool proceed(bool bUpdateTargets = true,
                       int64_t frameTime = 0) override;

  void setFaceLandmarks(const float* land_marks, int lenght);
  void setBlendLevel(float level) { this->blend_level_ = level; }
  void setImageTexture(std::shared_ptr<SourceImage> texture) {
    image_texture_ = texture;
  }

  void setTextureBounds(FrameBounds bounds) { texture_bounds_ = bounds; }
  void setHasFace(bool hasFace) { has_face_ = hasFace; }

 protected:
  FaceMakeupFilter();

 private:
  std::vector<GLuint> getFaceIndexs();
  std::vector<GLfloat> faceTextureCoordinates();

 private:
  std::vector<float> face_land_marks_;
  float blend_level_ = 0;  //[0. 0.5]
  bool has_face_ = false;
  //
  GLProgram* _filterProgram2;
  GLuint _filterPositionAttribute2;
  GLuint _filterTexCoordAttribute = 0;
  GLuint _filterTexCoordAttribute2 = 0;

  FrameBounds texture_bounds_;
  std::shared_ptr<SourceImage> image_texture_;
};

NS_GPUPIXEL_END
