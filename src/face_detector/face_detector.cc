/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "gpupixel/face_detector/face_detector.h"
#include <cassert>
#include "mars_vision/mars_defines.h"
#include "mars_vision/mars_face_landmarker.h"
#include "utils/filesystem.h"
#include "utils/logging.h"
#include "utils/util.h"

namespace gpupixel {

std::shared_ptr<FaceDetector> FaceDetector::Create() {
  return std::shared_ptr<FaceDetector>(new FaceDetector());
}

FaceDetector::FaceDetector() {
  auto path = Util::GetResourcePath() / "models";

  if (fs::exists(path)) {
    mars_face_detector_ = mars_vision::MarsFaceLandmarker::Create();

    mars_vision::FaceLandmarkerOptions landmarkerOptions;
    landmarkerOptions.model_path = path.string();
    landmarkerOptions.running_mode = mars_vision::RunningMode::VIDEO;

    mars_face_detector_->Init(landmarkerOptions);
  } else {
    LOG_ERROR("FaceDetector: models path not found: {}", path.string());
    assert(false && "FaceDetector: models path not found");
  }
}

std::vector<float> FaceDetector::Detect(const uint8_t* data,
                                        int width,
                                        int height,
                                        int stride,
                                        GPUPIXEL_MODE_FMT fmt,
                                        GPUPIXEL_FRAME_TYPE type) {
  mars_vision::MarsImage image;
  image.data = (uint8_t*)data;
  image.width = width == stride / 4 ? width : stride / 4;
  image.height = height;
  if (type == GPUPIXEL_FRAME_TYPE_RGBA) {
    image.format = mars_vision::MarsImageFormat::RGBA;
  } else if (type == GPUPIXEL_FRAME_TYPE_BGRA) {
    image.format = mars_vision::MarsImageFormat::BGRA;
  }
  image.stride = stride;
  image.rotate_type = mars_vision::RotateType::CLOCKWISE_0;
  image.timestamp = 0;

  std::vector<mars_vision::FaceLandmarkerResult> face_results;
  std::vector<float> landmarks;

  mars_face_detector_->Detect(image, face_results);
  // only support one face
  for (auto& result : face_results) {
    for (auto& point : result.key_points) {
      landmarks.push_back(point.x / width);
      landmarks.push_back(point.y / height);
    }
  }

  return landmarks;
}

}  // namespace gpupixel
