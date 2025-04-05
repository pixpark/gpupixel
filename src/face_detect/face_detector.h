/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include <stdlib.h>
#include <functional>
#include <vector>
#include "gpupixel_define.h"
namespace mars_face_kit {
class MarsFaceDetector;
}

namespace gpupixel {

class GPUPIXEL_API FaceDetector {
 public:
  static std::shared_ptr<FaceDetector> Create();
  ~FaceDetector();

  std::vector<float> Detect(const uint8_t* data,
                            int width,
                            int height,
                            int stride,
                            GPUPIXEL_MODE_FMT fmt,
                            GPUPIXEL_FRAME_TYPE type);

 private:
  FaceDetector();

  std::shared_ptr<mars_face_kit::MarsFaceDetector> mars_face_detector_;
};
}  // namespace gpupixel
