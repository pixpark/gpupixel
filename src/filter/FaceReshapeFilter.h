/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "Filter.h"

NS_GPUPIXEL_BEGIN

class FaceReshapeFilter : public Filter {
 public:
  static std::shared_ptr<FaceReshapeFilter> create();
  ~FaceReshapeFilter();
  bool init();
  virtual bool proceed(bool bUpdateTargets = true,
                       int64_t frameTime = 0) override;

  void setUniformWithLandmarks(const float* land_marks, int lenght);
  void setHasFace(bool has_face);

  void setFaceSlimLevel(float level);
  void setEyeZoomLevel(float level);

 protected:
  FaceReshapeFilter();
  float thinFaceDelta_ = 0;
  float eyeZoomLevel_ = 0;

  std::vector<float> face_land_marks_;
  int has_face_ = 0;
};

NS_GPUPIXEL_END
