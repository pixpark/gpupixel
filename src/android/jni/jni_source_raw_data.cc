#include <android/bitmap.h>
#include <jni.h>
#include <list>
#include <string>

#include "gpupixel_context.h"
#include "jni_helpers.h"
#include "source_raw_data.h"

using namespace gpupixel;

// Create new SourceRawData instance
extern "C" JNIEXPORT jlong JNICALL
Java_com_pixpark_gpupixel_GPUPixelSourceRawData_nativeCreate(JNIEnv* env,
                                                             jclass clazz) {
  auto source_raw_data = SourceRawData::Create();
  if (!source_raw_data) {
    return 0;
  }

  // 创建堆上的智能指针对象
  auto* ptr = new std::shared_ptr<SourceRawData>(source_raw_data);
  return reinterpret_cast<jlong>(ptr);
}

// Destroy SourceRawData instance
extern "C" JNIEXPORT void JNICALL
Java_com_pixpark_gpupixel_GPUPixelSourceRawData_nativeDestroy(
    JNIEnv* env,
    jclass clazz,
    jlong native_obj) {
  // 释放堆上分配的智能指针
  auto* ptr = reinterpret_cast<std::shared_ptr<SourceRawData>*>(native_obj);
  delete ptr;
}

// Release SourceRawData framebuffer
extern "C" JNIEXPORT void JNICALL
Java_com_pixpark_gpupixel_GPUPixelSourceRawData_nativeFinalize(
    JNIEnv* env,
    jclass clazz,
    jlong native_obj) {
  // 获取智能指针然后访问对象
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
