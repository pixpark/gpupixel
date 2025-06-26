//
//  mars-face-kit
//
//  Created by PixPark on 2024/11/18.
//

#pragma once

#include "mars_vision/mars_defines.h"
namespace mars_vision {

enum class RunningMode {
  // Run the vision detection on single image inputs.
  IMAGE = 0,
  // Run the vision detection on the decoded frames of an input video.
  VIDEO = 1,
};

struct FaceLandmarkerOptions {
  // The path to the model file.
  std::string model_path;

  // The running mode of the detection. Default to the image mode.
  // FaceLandmarker has two running modes:
  // 1) The image mode for recognizing face key points on single image
  // inputs. 2) The video mode for recognizing face key points on the decoded
  // frames of a
  //    video.
  RunningMode running_mode;
  // The maximum number of faces can be detected by the FaceLandmarker.
  int num_faces = 1;
  // The minimum confidence score of face presence score in the face landmark
  // detection.
  float min_face_presence_confidence = 0.5;
};

struct FaceLandmarkerResult {
  // face roi area
  Rect rect = {0.0f, 0.0f, 0.0f, 0.0f};
  // 111 normalized face key points
  std::vector<Point2d> key_points = {};
};

class MARS_VISION_API MarsFaceLandmarker {
 public:
  static std::shared_ptr<MarsFaceLandmarker> Create();

  // set models and resources root path
  virtual int Init(const FaceLandmarkerOptions& options) = 0;

  // detect face key points
  virtual int Detect(const MarsImage& image,
                     std::vector<FaceLandmarkerResult>& results) = 0;

#ifdef FB_DEBUG
  MarsImage roi_image;
#endif
};
}  // namespace mars_vision
