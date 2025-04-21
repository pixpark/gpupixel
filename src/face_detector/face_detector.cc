/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "gpupixel/face_detector/face_detector.h"
#include "mars_face_detector.h"
#include "utils/util.h"

namespace gpupixel {

std::shared_ptr<FaceDetector> FaceDetector::Create() {
  return std::shared_ptr<FaceDetector>(new FaceDetector());
}

FaceDetector::FaceDetector() {
  mars_face_detector_ = mars_face_kit::MarsFaceDetector::CreateFaceDetector();
  mars_face_detector_->Init(Util::GetResourcePath("models"));
}

std::vector<float> FaceDetector::Detect(const uint8_t* data,
                                        int width,
                                        int height,
                                        int stride,
                                        GPUPIXEL_MODE_FMT fmt,
                                        GPUPIXEL_FRAME_TYPE type) {
  mars_face_kit::MarsImage image;
  image.data = (uint8_t*)data;
  image.width = width == stride / 4 ? width : stride / 4;
  image.height = height;
  if (type == GPUPIXEL_FRAME_TYPE_RGBA) {
    image.pixel_format = mars_face_kit::PixelFormat::RGBA;
  } else if (type == GPUPIXEL_FRAME_TYPE_BGRA) {
    image.pixel_format = mars_face_kit::PixelFormat::BGRA;
  }
  image.width_step = width;
  image.rotate_type = mars_face_kit::CLOCKWISE_ROTATE_0;

  std::vector<mars_face_kit::FaceDetectionInfo> face_info;
  std::vector<float> landmarks;

  mars_face_detector_->Detect(image, face_info);
  if (face_info.size() > 0) {
    for (int i = 0; i < face_info[0].landmarks.size(); i++) {
      landmarks.push_back(face_info[0].landmarks[i].x / width);
      landmarks.push_back(face_info[0].landmarks[i].y / height);
    }

    // Calculate additional facial landmarks
    // Landmark 106: Center point between points 102 and 98
    auto point_x = (face_info[0].landmarks[102].x / width +
                    face_info[0].landmarks[98].x / width) /
                   2;
    auto point_y = (face_info[0].landmarks[102].y / height +
                    face_info[0].landmarks[98].y / height) /
                   2;
    landmarks.push_back(point_x);
    landmarks.push_back(point_y);

    // Landmark 107: Center point between points 35 and 65
    point_x = (face_info[0].landmarks[35].x / width +
               face_info[0].landmarks[65].x / width) /
              2;
    point_y = (face_info[0].landmarks[35].y / height +
               face_info[0].landmarks[65].y / height) /
              2;
    landmarks.push_back(point_x);
    landmarks.push_back(point_y);

    // Landmark 108: Center point between points 70 and 40
    point_x = (face_info[0].landmarks[70].x / width +
               face_info[0].landmarks[40].x / width) /
              2;
    point_y = (face_info[0].landmarks[70].y / height +
               face_info[0].landmarks[40].y / height) /
              2;
    landmarks.push_back(point_x);
    landmarks.push_back(point_y);

    // Landmark 109: Center point between points 5 and 80
    point_x = (face_info[0].landmarks[5].x / width +
               face_info[0].landmarks[80].x / width) /
              2;
    point_y = (face_info[0].landmarks[5].y / height +
               face_info[0].landmarks[80].y / height) /
              2;
    landmarks.push_back(point_x);
    landmarks.push_back(point_y);

    // Landmark 110: Center point between points 81 and 27
    point_x = (face_info[0].landmarks[81].x / width +
               face_info[0].landmarks[27].x / width) /
              2;
    point_y = (face_info[0].landmarks[81].y / height +
               face_info[0].landmarks[27].y / height) /
              2;
    landmarks.push_back(point_x);
    landmarks.push_back(point_y);
  }

  return landmarks;
}

}  // namespace gpupixel
