/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "face_detector.h"

#include "vnn_kit.h"
#include "vnn_face.h"

#include "util.h"
NS_GPUPIXEL_BEGIN


FaceDetector::FaceDetector() {
  //  init 
  VNN_SetLogLevel(VNN_LOG_LEVEL_ALL);
#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_ANDROID)
  auto model_path = Util::getResourcePath("face_mobile[1.0.0].vnnmodel");
#elif defined(GPUPIXEL_WIN) || defined(GPUPIXEL_MAC) || defined(GPUPIXEL_LINUX)
  auto model_path = Util::getResourcePath("face_pc[1.0.0].vnnmodel");
#endif
    const void *argv[] = {
      model_path.c_str(),
    };
  
  const int argc = sizeof(argv)/sizeof(argv[0]);
  VNN_Result  ret = VNN_Create_Face(&vnn_handle_, argc, argv);
}

FaceDetector::~FaceDetector() {
  if(vnn_handle_ > 0)
    VNN_Destroy_Face(&vnn_handle_);
}

int FaceDetector::RegCallback(FaceDetectorCallback callback) {
  _face_detector_callbacks.push_back(callback);
  return 0;
}
int FaceDetector::RegFacesDetectorCallback(FacesDetectorCallback facesDetectorCallback) {
  _faces_detector_callbacks.push_back(facesDetectorCallback);
  return 0;
}

int FaceDetector::Detect(const uint8_t* data, int width, int height, GPUPIXEL_MODE_FMT fmt, GPUPIXEL_FRAME_TYPE type) {
    if(vnn_handle_ == 0) {
        return -1;
    }
    
    VNN_Set_Face_Attr(vnn_handle_, "_use_278pts", &use_278pts);
    
    VNN_Image input;
    input.width = width;
    input.height = height;
    input.channels = 4;
    switch (type) {
        case GPUPIXEL_FRAME_TYPE_RGBA8888: {
            input.pix_fmt = VNN_PIX_FMT_BGRA8888;
        }
            break;
        case GPUPIXEL_FRAME_TYPE_YUVI420: {
            input.pix_fmt = VNN_PIX_FMT_YUVI420;
        }
            break;
        default:
            break;
    }
    
    input.data = (VNNVoidPtr)data;
    if(fmt == GPUPIXEL_MODE_FMT_VIDEO) {
        input.mode_fmt = VNN_MODE_FMT_VIDEO;
    }
    
    if(fmt == GPUPIXEL_MODE_FMT_PICTURE) {
        input.mode_fmt = VNN_MODE_FMT_PICTURE;
    }
    
    input.ori_fmt = VNN_ORIENT_FMT_DEFAULT;
    
    VNN_FaceFrameDataArr output;
    VNN_Result ret = VNN_Apply_Face_CPU(vnn_handle_, &input, &output);
    
    std::vector<std::vector<float>> facesMarr;
    if(output.facesNum > 0) {
        for (int faceIndex = 0; faceIndex < output.facesNum; faceIndex++) {
            std::vector<float> landmarks;
            for (int pointIndex = 0; pointIndex < output.facesArr[faceIndex].faceLandmarksNum; pointIndex++) {
                landmarks.push_back(output.facesArr[faceIndex].faceLandmarks[pointIndex].x);
                landmarks.push_back(output.facesArr[faceIndex].faceLandmarks[pointIndex].y);
            }
            
            // 106
            auto point_x = (output.facesArr[faceIndex].faceLandmarks[102].x + output.facesArr[faceIndex].faceLandmarks[98].x)/2;
            auto point_y = (output.facesArr[faceIndex].faceLandmarks[102].y + output.facesArr[faceIndex].faceLandmarks[98].y)/2;
            landmarks.push_back(point_x);
            landmarks.push_back(point_y);
            
            // 107
            point_x = (output.facesArr[faceIndex].faceLandmarks[35].x + output.facesArr[faceIndex].faceLandmarks[65].x)/2;
            point_y = (output.facesArr[faceIndex].faceLandmarks[35].y + output.facesArr[faceIndex].faceLandmarks[65].y)/2;
            landmarks.push_back(point_x);
            landmarks.push_back(point_y);
            
            
            // 108
            point_x = (output.facesArr[faceIndex].faceLandmarks[70].x + output.facesArr[faceIndex].faceLandmarks[40].x)/2;
            point_y = (output.facesArr[faceIndex].faceLandmarks[70].y + output.facesArr[faceIndex].faceLandmarks[40].y)/2;
            landmarks.push_back(point_x);
            landmarks.push_back(point_y);
            
            // 109
            point_x = (output.facesArr[faceIndex].faceLandmarks[5].x + output.facesArr[faceIndex].faceLandmarks[80].x)/2;
            point_y = (output.facesArr[faceIndex].faceLandmarks[5].y + output.facesArr[faceIndex].faceLandmarks[80].y)/2;
            landmarks.push_back(point_x);
            landmarks.push_back(point_y);
            
            // 110
            point_x = (output.facesArr[faceIndex].faceLandmarks[81].x + output.facesArr[faceIndex].faceLandmarks[27].x)/2;
            point_y = (output.facesArr[faceIndex].faceLandmarks[81].y + output.facesArr[faceIndex].faceLandmarks[27].y)/2;
            landmarks.push_back(point_x);
            landmarks.push_back(point_y);
            
            facesMarr.push_back(landmarks);
        }
    }
    
    // 单人人脸识别
    for(auto face_detector_cb : _face_detector_callbacks) {
        if (facesMarr.size() > 0) {
            face_detector_cb(facesMarr[0]);
        }
    }
    // 多人人脸识别
    for(auto faces_detector_cb : _faces_detector_callbacks) {
        if (facesMarr.size() > 0) {
            faces_detector_cb(facesMarr, (int)facesMarr.size());
        }
    }
    return 0;
}

NS_GPUPIXEL_END
