#if PLATFORM == PLATFORM_ANDROID

#include <android/bitmap.h>
#include <jni.h>
#include <string>
#include <list>
#include "GPUPixelContext.h"
#include "jni_helpers.h"
#include "libyuv.h"
#include "Filter.h"
#include "SourceCamera.h"
#include "SourceImage.h"
#include "SourceRawDataInput.h"
#include "TargetView.h"

USING_NS_GPUPIXEL
std::list<std::shared_ptr<Filter>>  filter_list_;

extern "C" jlong Java_com_pixpark_gpupixel_GPUPixel_nativeSourceImageNew(
    JNIEnv* env,
    jobject) {
  return (uintptr_t)(new SourceImage());
};

extern "C" void Java_com_pixpark_gpupixel_GPUPixel_nativeSourceImageDestroy(
    JNIEnv* env,
    jobject,
    jlong classId){};

extern "C" void Java_com_pixpark_gpupixel_GPUPixel_nativeSourceImageFinalize(
    JNIEnv* env,
    jobject,
    jlong classId) {
  ((SourceImage*)classId)->releaseFramebuffer(false);
};

extern "C" void Java_com_pixpark_gpupixel_GPUPixel_nativeSourceImageSetImage(
    JNIEnv* env,
    jobject,
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
    ((SourceImage*)classId)->setImage(info.width, info.height, pixels);
  }

  AndroidBitmap_unlockPixels(env, bitmap);
};

extern "C" jlong Java_com_pixpark_gpupixel_GPUPixel_nativeSourceCameraNew(
    JNIEnv* env,
    jobject) {
  return (uintptr_t)(new SourceCamera());
};

extern "C" void Java_com_pixpark_gpupixel_GPUPixel_nativeSourceCameraDestroy(
    JNIEnv* env,
    jobject,
    jlong classId){

};

extern "C" void Java_com_pixpark_gpupixel_GPUPixel_nativeSourceCameraFinalize(
    JNIEnv* env,
    jobject,
    jlong classId) {
  ((SourceCamera*)classId)->releaseFramebuffer(false);
};

extern "C" void Java_com_pixpark_gpupixel_GPUPixel_nativeSourceCameraSetFrame(
    JNIEnv* env,
    jobject,
    jlong classId,
    jint width,
    jint height,
    jintArray jdata,
    jint rotation) {
  jint* data = (jint*)(env->GetPrimitiveArrayCritical(jdata, 0));
  ((SourceCamera*)classId)
      ->setFrameData(width, height, data, (RotationMode)rotation);
  env->ReleasePrimitiveArrayCritical(jdata, data, 0);
};

extern "C" jlong Java_com_pixpark_gpupixel_GPUPixel_nativeSourceRawInputNew(
    JNIEnv* env,
    jobject) {
  return 0;
};

extern "C" void
Java_com_pixpark_gpupixel_GPUPixel_nativeSourceRawInputUploadBytes(
    JNIEnv* env,
    jobject,
    jlong classId,
    jintArray jPixel,
    jint width,
    jint height,
    jint stride) {
  // jbyte* pixel = (jbyte*) (env->GetPrimitiveArrayCritical(jPixel, 0));
  uint8_t* pixel = (uint8_t*)(env->GetPrimitiveArrayCritical(jPixel, 0));

  ((SourceRawDataInput*)classId)->uploadBytes(pixel, width, height, stride, 0);
  env->ReleasePrimitiveArrayCritical(jPixel, pixel, 0);
};

extern "C" void
Java_com_pixpark_gpupixel_GPUPixel_nativeSourceRawInputSetRotation(
    JNIEnv* env,
    jobject,
    jlong classId,
    jint rotation) {
  ((SourceRawDataInput*)classId)->setRotation((RotationMode)rotation);
};

extern "C" jlong Java_com_pixpark_gpupixel_GPUPixel_nativeSourceAddTarget(
    JNIEnv* env,
    jobject,
    jlong classId,
    jlong targetClassId,
    jint texID,
    jboolean isFilter) {
  Source* source = (Source*)classId;
  std::shared_ptr<Target> target =
      isFilter ? std::shared_ptr<Target>((Filter*)targetClassId)
               : std::shared_ptr<Target>((Target*)targetClassId);
  if (texID >= 0) {
    return (uintptr_t)(source->addTarget(target, texID)).get();
  } else {
    return (uintptr_t)(source->addTarget(target)).get();
  }
};

extern "C" void Java_com_pixpark_gpupixel_GPUPixel_nativeSourceRemoveTarget(
    JNIEnv* env,
    jobject,
    jlong classId,
    jlong targetClassId,
    jboolean isFilter) {
  Source* source = (Source*)classId;
  Target* target = isFilter ? dynamic_cast<Target*>((Filter*)targetClassId)
                            : (Target*)targetClassId;
  source->removeTarget(std::shared_ptr<Target>(target));
};

extern "C" void Java_com_pixpark_gpupixel_GPUPixel_nativeSourceRemoveAllTargets(
    JNIEnv* env,
    jobject,
    jlong classId) {
  ((Source*)classId)->removeAllTargets();
};

extern "C" jlong Java_com_pixpark_gpupixel_GPUPixel_nativeSourceProceed(
    JNIEnv* env,
    jobject,
    jlong classId,
    jboolean bUpdateTargets) {
  return ((Source*)classId)->proceed(bUpdateTargets, Util::nowTimeMs());
};

extern "C" jint
Java_com_pixpark_gpupixel_GPUPixel_nativeSourceGetRotatedFramebuferWidth(
    JNIEnv* env,
    jobject,
    jlong classId) {
  return ((Source*)classId)->getRotatedFramebufferWidth();
};

extern "C" jint
Java_com_pixpark_gpupixel_GPUPixel_nativeSourceGetRotatedFramebuferHeight(
    JNIEnv* env,
    jobject,
    jlong classId) {
  return ((Source*)classId)->getRotatedFramebufferHeight();
};

extern "C" jbyteArray
Java_com_pixpark_gpupixel_GPUPixel_nativeSourceCaptureAProcessedFrameData(
    JNIEnv* env,
    jobject,
    jlong classId,
    jlong upToFilterClassId,
    jint width,
    jint height) {
  unsigned char* processedFrameData =
      ((Source*)classId)
          ->captureAProcessedFrameData(
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

extern "C" jlong Java_com_pixpark_gpupixel_GPUPixel_nativeTargetViewNew(
    JNIEnv* env,
    jobject obj) {
  return (uintptr_t)(new TargetView());
};

extern "C" void Java_com_pixpark_gpupixel_GPUPixel_nativeTargetViewFinalize(
    JNIEnv* env,
    jobject,
    jlong classId){

};

extern "C" void
Java_com_pixpark_gpupixel_GPUPixel_nativeTargetViewOnSizeChanged(JNIEnv* env,
                                                                 jobject,
                                                                 jlong classId,
                                                                 jint width,
                                                                 jint height) {
  ((TargetView*)classId)->onSizeChanged(width, height);
};

extern "C" void Java_com_pixpark_gpupixel_GPUPixel_nativeTargetViewSetFillMode(
    JNIEnv* env,
    jobject,
    jlong classId,
    jint fillMode) {
  ((TargetView*)classId)->setFillMode((TargetView::FillMode)fillMode);
};


extern "C"
JNIEXPORT void JNICALL
Java_com_pixpark_gpupixel_GPUPixel_nativeTargetViewSetMirror(JNIEnv *env, jclass clazz,
                                                             jlong class_id, jboolean mirror) {
    ((TargetView*)class_id)->setMirror(mirror);
}

extern "C" jlong Java_com_pixpark_gpupixel_GPUPixel_nativeFilterCreate(
    JNIEnv* env,
    jobject obj,
    jstring jFilterClassName) {
  const char* filterClassName = env->GetStringUTFChars(jFilterClassName, 0);

  auto ft = Filter::create(filterClassName);
  filter_list_.push_back(ft);
  jlong ret = (jlong)ft.get();
  env->ReleaseStringUTFChars(jFilterClassName, filterClassName);
  return ret;
};

extern "C" void Java_com_pixpark_gpupixel_GPUPixel_nativeFilterDestroy(
    JNIEnv* env,
    jobject obj,
    jlong classId){
    for(auto ft : filter_list_) {
        if(classId == (jlong)ft.get()){
            filter_list_.remove(ft);
        }
    }
};

extern "C" void Java_com_pixpark_gpupixel_GPUPixel_nativeFilterFinalize(
    JNIEnv* env,
    jobject obj,
    jlong classId) {
  ((Filter*)classId)->releaseFramebuffer(false);
};

extern "C" void Java_com_pixpark_gpupixel_GPUPixel_nativeFilterSetPropertyFloat(
    JNIEnv* env,
    jobject obj,
    jlong classId,
    jstring jProperty,
    jfloat value) {
  const char* property = env->GetStringUTFChars(jProperty, 0);
  ((Filter*)classId)->setProperty(property, value);
  env->ReleaseStringUTFChars(jProperty, property);
};

extern "C" void Java_com_pixpark_gpupixel_GPUPixel_nativeFilterSetPropertyInt(
    JNIEnv* env,
    jobject obj,
    jlong classId,
    jstring jProperty,
    jint value) {
  const char* property = env->GetStringUTFChars(jProperty, 0);
  ((Filter*)classId)->setProperty(property, value);
  env->ReleaseStringUTFChars(jProperty, property);
};

extern "C" void
Java_com_pixpark_gpupixel_GPUPixel_nativeFilterSetPropertyString(
    JNIEnv* env,
    jobject obj,
    jlong classId,
    jstring jProperty,
    jstring jValue) {
  const char* property = env->GetStringUTFChars(jProperty, 0);
  const char* value = env->GetStringUTFChars(jValue, 0);
  ((Filter*)classId)->setProperty(property, value);
  env->ReleaseStringUTFChars(jProperty, property);
  env->ReleaseStringUTFChars(jValue, value);
};

extern "C" void Java_com_pixpark_gpupixel_GPUPixel_nativeContextInit(
    JNIEnv* env,
    jobject obj){

};

extern "C" void Java_com_pixpark_gpupixel_GPUPixel_nativeContextDestroy(
    JNIEnv* env,
    jobject obj) {
  GPUPixelContext::destroy();
};

extern "C" void Java_com_pixpark_gpupixel_GPUPixel_nativeContextPurge(
    JNIEnv* env,
    jobject obj) {
  GPUPixelContext::getInstance()->purge();
};

FILE* yuv_file_zz = NULL;
extern "C" void Java_com_pixpark_gpupixel_GPUPixel_nativeYUVtoRBGA(
    JNIEnv* env,
    jobject obj,
    jbyteArray yuv420sp,
    jint width,
    jint height,
    jintArray rgbOut) {

    jint* rgbData = (jint*)(env->GetPrimitiveArrayCritical(rgbOut, 0));
    jbyte* nv21 = (jbyte*)env->GetPrimitiveArrayCritical(yuv420sp, 0);

    uint8_t* src_rgba = (uint8_t*) malloc(width*height*4);
    libyuv::NV21ToABGR(reinterpret_cast<const uint8_t *>(nv21),
                     width,
                     reinterpret_cast<const uint8_t *>(nv21 + width * height),
                     width,
                     reinterpret_cast<uint8_t*>(src_rgba),
                     width*4,
                     width,
                     height);

    libyuv::ARGBRotate(src_rgba,
                     width*4,
                     reinterpret_cast<uint8_t*>(rgbData),
                     height*4,
                     width,
                     height,
                     libyuv::kRotate270);

    free(src_rgba);
    env->ReleasePrimitiveArrayCritical(rgbOut, rgbData, 0);
    env->ReleasePrimitiveArrayCritical(yuv420sp, nv21, 0);
}

extern "C" jint JNIEXPORT JNICALL JNI_OnLoad(JavaVM* jvm, void* reserved) {
  SetJVM(jvm);
  return JNI_VERSION_1_6;
}

extern "C" void JNIEXPORT JNICALL JNI_OnUnLoad(JavaVM* jvm, void* reserved) {
//  RTC_CHECK(rtc::CleanupSSL()) << "Failed to CleanupSSL()";
}

#endif