/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "filter.h"


namespace gpupixel {
class GPUPIXEL_API FaceReshapeFilter : public Filter {
 public:
  static std::shared_ptr<FaceReshapeFilter> create();
  ~FaceReshapeFilter();
  bool init();
  virtual bool doRender(bool updateSinks = true) override;

  void setFaceSlimLevel(float level);
  void setEyeZoomLevel(float level);
  void SetFaceLandmarks(std::vector<float> landmarks);
 protected:
  FaceReshapeFilter();
  float thinFaceDelta_ = 0;
  float bigEyeDelta_ = 0;

  std::vector<float> face_land_marks_;
  int has_face_ = 0;
};

}
