/*
 * GPUPixel
 *
 * Created by PixPark on 2023/10/30.
 * Copyright © 2023 PixPark. All rights reserved.
 */

#include <jni.h>
#include <list>
#include <memory>
#include <vector>

#include "face_detector.h"
#include "jni_helpers.h"

using namespace gpupixel;

// Global face detector list for managing face detector lifecycle
std::list<std::shared_ptr<FaceDetector>> face_detector_list_;

// Create face detector
extern "C" jlong
Java_com_pixpark_gpupixel_FaceDetector_nativeFaceDetectorCreate(JNIEnv* env,
                                                                jclass obj) {
  auto detector = FaceDetector::Create();
  face_detector_list_.push_back(detector);
  return (jlong)detector.get();
}

// Destroy face detector
extern "C" void
Java_com_pixpark_gpupixel_FaceDetector_nativeFaceDetectorDestroy(
    JNIEnv* env,
    jclass obj,
    jlong classId) {
  for (auto detector : face_detector_list_) {
    if (classId == (jlong)detector.get()) {
      face_detector_list_.remove(detector);
      break;
    }
  }
}

// Detect face landmarks
extern "C" jfloatArray
Java_com_pixpark_gpupixel_FaceDetector_nativeFaceDetectorDetect(
    JNIEnv* env,
    jclass obj,
    jlong classId,
    jbyteArray jData,
    jint width,
    jint height,
    jint stride,
    jint format,
    jint frameType) {
  // Get Java byte array
  jbyte* data = env->GetByteArrayElements(jData, nullptr);

  // Call C++ method to detect face landmarks
  std::vector<float> landmarks =
      ((FaceDetector*)classId)
          ->Detect((const uint8_t*)data, width, height, stride,
                   (GPUPIXEL_MODE_FMT)format, (GPUPIXEL_FRAME_TYPE)frameType);

  // Release Java byte array
  env->ReleaseByteArrayElements(jData, data, JNI_ABORT);

  // Create Java float array
  jfloatArray result = env->NewFloatArray(landmarks.size());
  if (result == nullptr) {
    return nullptr;
  }

  // Fill Java float array
  env->SetFloatArrayRegion(result, 0, landmarks.size(), landmarks.data());

  return result;
}
