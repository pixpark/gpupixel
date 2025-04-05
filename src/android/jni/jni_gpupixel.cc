#if PLATFORM == PLATFORM_ANDROID

#include <android/bitmap.h>
#include <face_reshape_filter.h>
#include <jni.h>
#include <list>
#include <string>
#include "face_detector.h"
#include "filter.h"
#include "gpupixel_context.h"
#include "jni_helpers.h"
#include "libyuv.h"
#include "sink_render.h"
#include "source_camera.h"
#include "source_image.h"
#include "source_raw_data.h"

using namespace gpupixel;
std::list<std::shared_ptr<Filter>> filter_list_;
std::shared_ptr<gpupixel::FaceDetector> face_detector_;

extern "C" jlong Java_com_pixpark_gpupixel_GPUPixel_nativeSourceImageNew(
    JNIEnv* env,
    jclass) {
  return (uintptr_t)(new SourceImage());
};

extern "C" void Java_com_pixpark_gpupixel_GPUPixel_nativeSourceImageDestroy(
    JNIEnv* env,
    jclass,
    jlong classId) {};

extern "C" void Java_com_pixpark_gpupixel_GPUPixel_nativeSourceImageFinalize(
    JNIEnv* env,
    jclass,
    jlong classId) {
  ((SourceImage*)classId)->ReleaseFramebuffer(false);
};

extern "C" void Java_com_pixpark_gpupixel_GPUPixel_nativeSourceImageSetImage(
    JNIEnv* env,
    jclass,
    jlong classId,
    jobject bitmap) {
  char* pData = 0;
  AndroidBitmapInfo info;
  void* pixels;
  if ((AndroidBitmap_getInfo(env, bitmap, &info)) < 0) {
    // ERROR
    return;
  }

  if ((AndroidBitmap_lockPixels(env, bitmap, &pixels)) >= 0) {
    ((SourceImage*)classId)
        ->Init(info.width, info.height, 4, (const unsigned char*)pixels);
  }

  AndroidBitmap_unlockPixels(env, bitmap);
};

extern "C" jlong Java_com_pixpark_gpupixel_GPUPixel_nativeSourceCameraNew(
    JNIEnv* env,
    jclass) {
  return (uintptr_t)(new SourceCamera());
};

extern "C" void Java_com_pixpark_gpupixel_GPUPixel_nativeSourceCameraDestroy(
    JNIEnv* env,
    jclass,
    jlong classId) {

};

extern "C" void Java_com_pixpark_gpupixel_GPUPixel_nativeSourceCameraFinalize(
    JNIEnv* env,
    jclass,
    jlong classId) {
  ((SourceCamera*)classId)->ReleaseFramebuffer(false);
};

extern "C" void Java_com_pixpark_gpupixel_GPUPixel_nativeSourceCameraSetFrame(
    JNIEnv* env,
    jclass,
    jlong classId,
    jint width,
    jint height,
    jbyteArray jdata,
    jint rotation) {
  jbyte* data = env->GetByteArrayElements(jdata, NULL);
  ((SourceCamera*)classId)
      ->SetFrameData(width, height, data, (RotationMode)rotation);
  env->ReleaseByteArrayElements(jdata, data, 0);
};

extern "C" jlong Java_com_pixpark_gpupixel_GPUPixel_nativeSourceRawDataNew(
    JNIEnv* env,
    jclass) {
  return 0;
};

extern "C" void
Java_com_pixpark_gpupixel_GPUPixel_nativeSourceRawDataUploadBytes(
    JNIEnv* env,
    jclass,
    jlong classId,
    jintArray jPixel,
    jint width,
    jint height,
    jint stride) {
  jint* pixel = env->GetIntArrayElements(jPixel, 0);
  ((SourceRawData*)classId)
      ->ProcessData((uint8_t*)pixel, width, height, stride,
                    GPUPIXEL_FRAME_TYPE_RGBA);
  env->ReleaseIntArrayElements(jPixel, pixel, 0);
};

extern "C" void
Java_com_pixpark_gpupixel_GPUPixel_nativeSourceRawDataSetRotation(
    JNIEnv* env,
    jclass,
    jlong classId,
    jint rotation) {
  ((SourceRawData*)classId)->SetRotation((RotationMode)rotation);
};

extern "C" jlong Java_com_pixpark_gpupixel_GPUPixel_nativeSourceAddSink(
    JNIEnv* env,
    jclass,
    jlong classId,
    jlong sinkClassId,
    jint texID,
    jboolean isFilter) {
  Source* source = (Source*)classId;
  std::shared_ptr<Sink> sink = isFilter
                                   ? std::shared_ptr<Sink>((Filter*)sinkClassId)
                                   : std::shared_ptr<Sink>((Sink*)sinkClassId);
  if (texID >= 0) {
    return (uintptr_t)(source->AddSink(sink, texID)).get();
  } else {
    return (uintptr_t)(source->AddSink(sink)).get();
  }
};

extern "C" void Java_com_pixpark_gpupixel_GPUPixel_nativeSourceRemoveSink(
    JNIEnv* env,
    jclass,
    jlong classId,
    jlong sinkClassId,
    jboolean isFilter) {
  Source* source = (Source*)classId;
  Sink* sink =
      isFilter ? dynamic_cast<Sink*>((Filter*)sinkClassId) : (Sink*)sinkClassId;
  source->RemoveSink(std::shared_ptr<Sink>(sink));
};

extern "C" void Java_com_pixpark_gpupixel_GPUPixel_nativeSourceRemoveAllSinks(
    JNIEnv* env,
    jclass,
    jlong classId) {
  ((Source*)classId)->RemoveAllSinks();
};

extern "C" jboolean Java_com_pixpark_gpupixel_GPUPixel_nativeSourceProceed(
    JNIEnv* env,
    jclass,
    jlong classId,
    jboolean updateSinks) {
  return ((Source*)classId)->DoRender(updateSinks);
};

extern "C" jint
Java_com_pixpark_gpupixel_GPUPixel_nativeSourceGetRotatedFramebuferWidth(
    JNIEnv* env,
    jclass,
    jlong classId) {
  return ((Source*)classId)->GetRotatedFramebufferWidth();
};

extern "C" jint
Java_com_pixpark_gpupixel_GPUPixel_nativeSourceGetRotatedFramebuferHeight(
    JNIEnv* env,
    jclass,
    jlong classId) {
  return ((Source*)classId)->GetRotatedFramebufferHeight();
};

extern "C" jbyteArray
Java_com_pixpark_gpupixel_GPUPixel_nativeSourceGetProcessedFrameData(
    JNIEnv* env,
    jclass,
    jlong classId,
    jlong upToFilterClassId,
    jint width,
    jint height) {
  unsigned char* processedFrameData =
      ((Source*)classId)
          ->GetProcessedFrameData(
              std::shared_ptr<Filter>((Filter*)upToFilterClassId), width,
              height);
  int frameSize = width * height * 4 * sizeof(unsigned char);

  jbyteArray jresult = NULL;
  if (processedFrameData) {
    jbyte* by = (jbyte*)processedFrameData;
    jresult = env->NewByteArray(frameSize);
    env->SetByteArrayRegion(jresult, 0, frameSize, by);
    delete[] processedFrameData;
    processedFrameData = 0;
  }
  return jresult;
};

extern "C" jlong Java_com_pixpark_gpupixel_GPUPixel_nativeSinkRender(
    JNIEnv* env,
    jclass obj) {
  return (uintptr_t)(new SinkRender());
};

extern "C" void Java_com_pixpark_gpupixel_GPUPixel_nativeSinkRenderFinalize(
    JNIEnv* env,
    jclass,
    jlong classId) {

};

extern "C" void
Java_com_pixpark_gpupixel_GPUPixel_nativeSinkRenderOnSizeChanged(JNIEnv* env,
                                                                 jclass obj,
                                                                 jlong classId,
                                                                 jint width,
                                                                 jint height) {
  ((SinkRender*)classId)->SetRenderSize(width, height);
};

extern "C" void Java_com_pixpark_gpupixel_GPUPixel_nativeSinkRenderSetFillMode(
    JNIEnv* env,
    jclass,
    jlong classId,
    jint fillMode) {
  ((SinkRender*)classId)->SetFillMode((SinkRender::FillMode)fillMode);
};

extern "C" JNIEXPORT void JNICALL
Java_com_pixpark_gpupixel_GPUPixel_nativeSinkRenderSetMirror(JNIEnv* env,
                                                             jclass clazz,
                                                             jlong class_id,
                                                             jboolean mirror) {
  ((SinkRender*)class_id)->SetMirror(mirror);
}

extern "C" jlong Java_com_pixpark_gpupixel_GPUPixel_nativeFilterCreate(
    JNIEnv* env,
    jclass obj,
    jstring jFilterClassName) {
  const char* filterClassName = env->GetStringUTFChars(jFilterClassName, 0);

  auto ft = Filter::Create(filterClassName);
  filter_list_.push_back(ft);
  jlong ret = (jlong)ft.get();
  env->ReleaseStringUTFChars(jFilterClassName, filterClassName);
  return ret;
};

extern "C" void Java_com_pixpark_gpupixel_GPUPixel_nativeFilterDestroy(
    JNIEnv* env,
    jclass obj,
    jlong classId) {
  for (auto ft : filter_list_) {
    if (classId == (jlong)ft.get()) {
      filter_list_.remove(ft);
    }
  }
};

extern "C" void Java_com_pixpark_gpupixel_GPUPixel_nativeFilterFinalize(
    JNIEnv* env,
    jclass obj,
    jlong classId) {
  ((Filter*)classId)->ReleaseFramebuffer(false);
};

extern "C" void Java_com_pixpark_gpupixel_GPUPixel_nativeFilterSetPropertyFloat(
    JNIEnv* env,
    jclass obj,
    jlong classId,
    jstring jProperty,
    jfloat value) {
  const char* property = env->GetStringUTFChars(jProperty, 0);
  ((Filter*)classId)->SetProperty(property, value);
  env->ReleaseStringUTFChars(jProperty, property);
};

extern "C" void Java_com_pixpark_gpupixel_GPUPixel_nativeFilterSetPropertyInt(
    JNIEnv* env,
    jclass obj,
    jlong classId,
    jstring jProperty,
    jint value) {
  const char* property = env->GetStringUTFChars(jProperty, 0);
  ((Filter*)classId)->SetProperty(property, value);
  env->ReleaseStringUTFChars(jProperty, property);
};

extern "C" void
Java_com_pixpark_gpupixel_GPUPixel_nativeFilterSetPropertyString(
    JNIEnv* env,
    jclass obj,
    jlong classId,
    jstring jProperty,
    jstring jValue) {
  const char* property = env->GetStringUTFChars(jProperty, 0);
  const char* value = env->GetStringUTFChars(jValue, 0);
  ((Filter*)classId)->SetProperty(property, value);
  env->ReleaseStringUTFChars(jProperty, property);
  env->ReleaseStringUTFChars(jValue, value);
};

extern "C" void Java_com_pixpark_gpupixel_GPUPixel_nativeContextInit(
    JNIEnv* env,
    jclass obj) {

};

extern "C" void Java_com_pixpark_gpupixel_GPUPixel_nativeContextDestroy(
    JNIEnv* env,
    jclass obj) {
  GPUPixelContext::Destroy();
};

extern "C" void Java_com_pixpark_gpupixel_GPUPixel_nativeContextClean(
    JNIEnv* env,
    jclass obj) {
  GPUPixelContext::GetInstance()->Clean();
};

extern "C" void Java_com_pixpark_gpupixel_GPUPixel_nativeYUVtoRBGA(
    JNIEnv* env,
    jclass obj,
    jbyteArray yuv420sp,
    jint width,
    jint height,
    jbyteArray rgbOut) {
  jbyte* rgbData = (jbyte*)(env->GetPrimitiveArrayCritical(rgbOut, 0));
  jbyte* nv21 = (jbyte*)env->GetPrimitiveArrayCritical(yuv420sp, 0);

  uint8_t* src_rgba = (uint8_t*)malloc(width * height * 4);
  libyuv::NV21ToABGR(reinterpret_cast<const uint8_t*>(nv21), width,
                     reinterpret_cast<const uint8_t*>(nv21 + width * height),
                     width, src_rgba, width * 4, width, height);

  // Restore rotation logic, Android captured images need to be rotated 270
  // degrees
  uint8_t* rotated_rgba = (uint8_t*)malloc(width * height * 4);
  libyuv::ARGBRotate(src_rgba, width * 4, rotated_rgba, height * 4, width,
                     height, libyuv::kRotate270);

  // Copy rotated data to output buffer
  memcpy(rgbData, rotated_rgba, width * height * 4);

  free(src_rgba);
  free(rotated_rgba);
  env->ReleasePrimitiveArrayCritical(rgbOut, rgbData, 0);
  env->ReleasePrimitiveArrayCritical(yuv420sp, nv21, 0);
}

extern "C" JNIEXPORT void JNICALL
Java_com_pixpark_gpupixel_GPUPixel_nativeFilterSetPropertyFloatArray(
    JNIEnv* env,
    jclass clazz,
    jlong class_id,
    jstring jProperty,
    jfloatArray jarray) {
  const char* property = env->GetStringUTFChars(jProperty, 0);
  jsize length = env->GetArrayLength(jarray);

  // Get pointer to the original Java array
  jfloat* c_array = env->GetFloatArrayElements(jarray, nullptr);
  std::vector<float> vector;
  for (int i = 0; i < length; i++) {
    vector.push_back(c_array[i]);
  }
  ((Filter*)class_id)->SetProperty(property, vector);
  env->ReleaseStringUTFChars(jProperty, property);
  // Release Java array memory
  env->ReleaseFloatArrayElements(jarray, c_array, JNI_ABORT);
}

extern "C" jint JNIEXPORT JNICALL JNI_OnLoad(JavaVM* jvm, void* reserved) {
  SetJVM(jvm);
  return JNI_VERSION_1_6;
}

extern "C" void JNIEXPORT JNICALL JNI_OnUnLoad(JavaVM* jvm, void* reserved) {
  //  RTC_CHECK(rtc::CleanupSSL()) << "Failed to CleanupSSL()";
}

// FaceDetector JNI methods
extern "C" jlong Java_com_pixpark_gpupixel_GPUPixel_nativeFaceDetectorCreate(
    JNIEnv* env,
    jclass) {
  if (!face_detector_) {
    face_detector_ = std::make_shared<FaceDetector>();
  }
  return (jlong)(face_detector_.get());
};

extern "C" void Java_com_pixpark_gpupixel_GPUPixel_nativeFaceDetectorDestroy(
    JNIEnv* env,
    jclass,
    jlong classId) {
  face_detector_.reset();
};

extern "C" jfloatArray
Java_com_pixpark_gpupixel_GPUPixel_nativeFaceDetectorDetect(JNIEnv* env,
                                                            jclass,
                                                            jlong classId,
                                                            jbyteArray jdata,
                                                            jint width,
                                                            jint height,
                                                            jint format,
                                                            jint frameType) {
  jbyte* data = env->GetByteArrayElements(jdata, nullptr);
  std::vector<float> landmarks =
      ((FaceDetector*)classId)
          ->Detect((const uint8_t*)data, width, height,
                   (GPUPIXEL_MODE_FMT)format, (GPUPIXEL_FRAME_TYPE)frameType);
  env->ReleaseByteArrayElements(jdata, data, JNI_ABORT);

  jfloatArray result = env->NewFloatArray(landmarks.size());
  if (result) {
    env->SetFloatArrayRegion(result, 0, landmarks.size(), landmarks.data());
  }
  return result;
};

#endif
