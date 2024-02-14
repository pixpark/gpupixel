/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "face_detector.h"

#import "vnn_kit.h"
#import "vnn_face.h"

#include "Util.h"

NS_GPUPIXEL_BEGIN
std::vector<FaceDetectorCallback> FaceDetector::_face_detector_callbacks;

int FaceDetector::RegCallback(FaceDetectorCallback callback) {
  _face_detector_callbacks.push_back(callback);
  return 0;
}

FaceDetector::FaceDetector() {
  //  init 
  VNN_SetLogLevel(VNN_LOG_LEVEL_ALL);
  auto model_path = Util::getResourcePath("face_mobile[1.0.0].vnnmodel");
    const void *argv[] = {
      model_path.c_str(),
    };
  
  const int argc = sizeof(argv)/sizeof(argv[0]);
  VNN_Result  ret = VNN_Create_Face(&vnn_handle_, argc, argv);
}

FaceDetector::~FaceDetector() {
  
}

int FaceDetector::Detect(const uint8_t* data,
                    int width,
                    int height,
                    GPUPIXEL_FRAME_TYPE type) {
  if(_face_detector_callbacks.empty()) {
    return 0;
  }
  
  if(vnn_handle_ == 0) {
    return -1;
  }
  
  VNN_Set_Face_Attr(vnn_handle_, "_use_278pts", &use_278pts);

  VNN_Image input;
  input.width = width;
  input.height = height;
  input.channels = 4;
  if(type == GPUPIXEL_FRAME_TYPE_RGBA8888)
    input.pix_fmt = VNN_PIX_FMT_BGRA8888;
  else if(type == GPUPIXEL_FRAME_TYPE_YUVI420) {
    input.pix_fmt = VNN_PIX_FMT_YUVI420;
  }
  input.data = (VNNVoidPtr)data;
  input.mode_fmt = VNN_MODE_FMT_VIDEO;
  input.ori_fmt = VNN_ORIENT_FMT_DEFAULT;

  VNN_FaceFrameDataArr output;
  VNN_Apply_Face_CPU(vnn_handle_, &input, &output);
 
  if(output.facesNum > 0) {
    std::vector<float> landmarks;
    for (int i = 0; i < output.facesArr[0].faceLandmarksNum; i++) {
      landmarks.push_back(output.facesArr[0].faceLandmarks[i].x);
      landmarks.push_back(output.facesArr[0].faceLandmarks[i].y);
    }

    // 106
    auto point_x = (output.facesArr[0].faceLandmarks[102].x + output.facesArr[0].faceLandmarks[98].x)/2;
    auto point_y = (output.facesArr[0].faceLandmarks[102].y + output.facesArr[0].faceLandmarks[98].y)/2;
    landmarks.push_back(point_x);
    landmarks.push_back(point_y);
    
    // 107
    point_x = (output.facesArr[0].faceLandmarks[35].x + output.facesArr[0].faceLandmarks[65].x)/2;
    point_y = (output.facesArr[0].faceLandmarks[35].y + output.facesArr[0].faceLandmarks[65].y)/2;
    landmarks.push_back(point_x);
    landmarks.push_back(point_y);
    
    
    // 108
    point_x = (output.facesArr[0].faceLandmarks[70].x + output.facesArr[0].faceLandmarks[40].x)/2;
    point_y = (output.facesArr[0].faceLandmarks[70].y + output.facesArr[0].faceLandmarks[40].y)/2;
    landmarks.push_back(point_x);
    landmarks.push_back(point_y);
    
    // 109
    point_x = (output.facesArr[0].faceLandmarks[5].x + output.facesArr[0].faceLandmarks[80].x)/2;
    point_y = (output.facesArr[0].faceLandmarks[5].y + output.facesArr[0].faceLandmarks[80].y)/2;
    landmarks.push_back(point_x);
    landmarks.push_back(point_y);

    // 110
    point_x = (output.facesArr[0].faceLandmarks[81].x + output.facesArr[0].faceLandmarks[27].x)/2;
    point_y = (output.facesArr[0].faceLandmarks[81].y + output.facesArr[0].faceLandmarks[27].y)/2;
    landmarks.push_back(point_x);
    landmarks.push_back(point_y);
    
    // do callbck
    for(auto cb : _face_detector_callbacks) {
      cb(landmarks);
    }
    return 0;
  }
}

NS_GPUPIXEL_END
