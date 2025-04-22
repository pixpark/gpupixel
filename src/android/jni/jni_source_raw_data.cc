#include <android/bitmap.h>
#include <jni.h>
#include <list>
#include <string>

#include "android/jni/jni_helpers.h"
#include "core/gpupixel_context.h"
#include "gpupixel/source/source_raw_data.h"

using namespace gpupixel;

// Create new SourceRawData instance
extern "C" JNIEXPORT jlong JNICALL
Java_com_pixpark_gpupixel_GPUPixelSourceRawData_nativeCreate(JNIEnv* env,
                                                             jclass clazz) {
  auto source_raw_data = SourceRawData::Create();
  if (!source_raw_data) {
    return 0;
  }

  // Create smart pointer object on heap
  auto* ptr = new std::shared_ptr<SourceRawData>(source_raw_data);
  return reinterpret_cast<jlong>(ptr);
}

// Destroy SourceRawData instance
extern "C" JNIEXPORT void JNICALL
Java_com_pixpark_gpupixel_GPUPixelSourceRawData_nativeDestroy(
    JNIEnv* env,
    jclass clazz,
    jlong native_obj) {
  // Release smart pointer allocated on heap
  auto* ptr = reinterpret_cast<std::shared_ptr<SourceRawData>*>(native_obj);
  delete ptr;
}

// Release SourceRawData framebuffer
extern "C" JNIEXPORT void JNICALL
Java_com_pixpark_gpupixel_GPUPixelSourceRawData_nativeFinalize(
    JNIEnv* env,
    jclass clazz,
    jlong native_obj) {
  // Get smart pointer and access object
  auto* ptr = reinterpret_cast<std::shared_ptr<SourceRawData>*>(native_obj);
  if (ptr && *ptr) {
    (*ptr)->ReleaseFramebuffer(false);
  }
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
  auto* ptr = reinterpret_cast<std::shared_ptr<SourceRawData>*>(native_obj);
  if (!ptr || !*ptr) {
    return;
  }

  jbyte* bytes = env->GetByteArrayElements(data, NULL);

  (*ptr)->ProcessData((uint8_t*)bytes, width, height, stride,
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
  auto* ptr = reinterpret_cast<std::shared_ptr<SourceRawData>*>(native_obj);
  if (ptr && *ptr) {
    (*ptr)->SetRotation((RotationMode)rotation);
  }
}
