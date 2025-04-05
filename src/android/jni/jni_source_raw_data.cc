#include <android/bitmap.h>
#include <jni.h>
#include <list>
#include <string>

#include "gpupixel_context.h"
#include "jni_helpers.h"
#include "source_raw_data.h"

using namespace gpupixel;

// Maintain all created SourceRawData objects
static std::list<std::shared_ptr<SourceRawData>> source_raw_data_list_;

// Create new SourceRawData instance
extern "C" JNIEXPORT jlong JNICALL
Java_com_pixpark_gpupixel_GPUPixelSourceRawData_nativeCreate(JNIEnv* env,
                                                             jclass clazz) {
  auto source_raw_data = SourceRawData::Create();
  if (!source_raw_data) {
    return 0;
  }
  source_raw_data_list_.push_back(source_raw_data);
  return (jlong)(source_raw_data.get());
}

// Destroy SourceRawData instance
extern "C" JNIEXPORT void JNICALL
Java_com_pixpark_gpupixel_GPUPixelSourceRawData_nativeDestroy(
    JNIEnv* env,
    jclass clazz,
    jlong native_obj) {
  for (auto srd : source_raw_data_list_) {
    if ((jlong)srd.get() == native_obj) {
      source_raw_data_list_.remove(srd);
      break;
    }
  }
}

// Release SourceRawData framebuffer
extern "C" JNIEXPORT void JNICALL
Java_com_pixpark_gpupixel_GPUPixelSourceRawData_nativeFinalize(
    JNIEnv* env,
    jclass clazz,
    jlong native_obj) {
  ((SourceRawData*)native_obj)->ReleaseFramebuffer(false);
}

// Process data (unified interface) - only handles byte[]
extern "C" JNIEXPORT void JNICALL
Java_com_pixpark_gpupixel_GPUPixelSourceRawData_nativeProcessData(
    JNIEnv* env,
    jclass clazz,
    jlong native_obj,
    jbyteArray data,
    jint width,
    jint height,
    jint stride,
    jint type) {
  jbyte* bytes = env->GetByteArrayElements(data, NULL);

  ((SourceRawData*)native_obj)
      ->ProcessData((uint8_t*)bytes, width, height, stride,
                    (GPUPIXEL_FRAME_TYPE)type);

  env->ReleaseByteArrayElements(data, bytes, 0);
}

// Set rotation mode
extern "C" JNIEXPORT void JNICALL
Java_com_pixpark_gpupixel_GPUPixelSourceRawData_nativeSetRotation(
    JNIEnv* env,
    jclass clazz,
    jlong native_obj,
    jint rotation) {
  ((SourceRawData*)native_obj)->SetRotation((RotationMode)rotation);
}
