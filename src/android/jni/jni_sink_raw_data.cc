#include <jni.h>
#include <list>
#include <string>

#include "android/jni/jni_helpers.h"
#include "core/gpupixel_context.h"
#include "gpupixel/sink/sink_raw_data.h"

using namespace gpupixel;

// Create a new SinkRawData instance
extern "C" JNIEXPORT jlong JNICALL
Java_com_pixpark_gpupixel_GPUPixelSinkRawData_nativeCreate(JNIEnv* env,
                                                           jclass clazz) {
  auto sink_raw_data = SinkRawData::Create();
  if (!sink_raw_data) {
    return 0;
  }

  // Create shared_ptr on heap
  auto* ptr = new std::shared_ptr<SinkRawData>(sink_raw_data);
  return reinterpret_cast<jlong>(ptr);
}

// Destroy a SinkRawData instance
extern "C" JNIEXPORT void JNICALL
Java_com_pixpark_gpupixel_GPUPixelSinkRawData_nativeDestroy(JNIEnv* env,
                                                            jclass clazz,
                                                            jlong native_obj) {
  // Free the heap-allocated shared_ptr
  auto* ptr = reinterpret_cast<std::shared_ptr<SinkRawData>*>(native_obj);
  delete ptr;
}

// Finalize SinkRawData resources
extern "C" JNIEXPORT void JNICALL
Java_com_pixpark_gpupixel_GPUPixelSinkRawData_nativeFinalize(JNIEnv* env,
                                                             jclass clazz,
                                                             jlong native_obj) {
  // Add cleanup code here if needed
  auto* ptr = reinterpret_cast<std::shared_ptr<SinkRawData>*>(native_obj);
  if (ptr && *ptr) {
    // Add special cleanup code here if needed
  }
}

// Get width
extern "C" JNIEXPORT jint JNICALL
Java_com_pixpark_gpupixel_GPUPixelSinkRawData_nativeGetWidth(JNIEnv* env,
                                                             jclass clazz,
                                                             jlong native_obj) {
  auto* ptr = reinterpret_cast<std::shared_ptr<SinkRawData>*>(native_obj);
  return ptr && *ptr ? (*ptr)->GetWidth() : 0;
}

// Get height
extern "C" JNIEXPORT jint JNICALL
Java_com_pixpark_gpupixel_GPUPixelSinkRawData_nativeGetHeight(
    JNIEnv* env,
    jclass clazz,
    jlong native_obj) {
  auto* ptr = reinterpret_cast<std::shared_ptr<SinkRawData>*>(native_obj);
  return ptr && *ptr ? (*ptr)->GetHeight() : 0;
}

// Get RGBA buffer data
extern "C" JNIEXPORT jbyteArray JNICALL
Java_com_pixpark_gpupixel_GPUPixelSinkRawData_nativeGetRgbaBuffer(
    JNIEnv* env,
    jclass clazz,
    jlong native_obj) {
  auto* ptr = reinterpret_cast<std::shared_ptr<SinkRawData>*>(native_obj);
  if (!ptr || !*ptr) {
    return NULL;
  }

  const uint8_t* buffer = (*ptr)->GetRgbaBuffer();
  if (!buffer) {
    return NULL;
  }

  int width = (*ptr)->GetWidth();
  int height = (*ptr)->GetHeight();

  // Check if width and height are valid
  if (width <= 0 || height <= 0) {
    return NULL;
  }

  // Check for potential overflow
  if (width > INT_MAX / height || width * height > INT_MAX / 4) {
    return NULL;
  }

  int size = width * height * 4;  // RGBA = 4 bytes per pixel

  jbyteArray result = env->NewByteArray(size);
  if (!result) {
    return NULL;
  }

  env->SetByteArrayRegion(result, 0, size, (jbyte*)buffer);

  return result;
}

// Get I420 buffer data
extern "C" JNIEXPORT jbyteArray JNICALL
Java_com_pixpark_gpupixel_GPUPixelSinkRawData_nativeGetI420Buffer(
    JNIEnv* env,
    jclass clazz,
    jlong native_obj) {
  auto* ptr = reinterpret_cast<std::shared_ptr<SinkRawData>*>(native_obj);
  if (!ptr || !*ptr) {
    return NULL;
  }

  const uint8_t* buffer = (*ptr)->GetI420Buffer();
  if (!buffer) {
    return NULL;
  }

  int width = (*ptr)->GetWidth();
  int height = (*ptr)->GetHeight();

  // Check if width and height are valid
  if (width <= 0 || height <= 0) {
    return NULL;
  }

  // Check for potential overflow
  if (width > INT_MAX / height || width * height > INT_MAX / 2) {
    return NULL;
  }

  int size = width * height * 3 / 2;  // I420 size = width * height * 1.5

  jbyteArray result = env->NewByteArray(size);
  if (!result) {
    return NULL;
  }

  env->SetByteArrayRegion(result, 0, size, (jbyte*)buffer);

  return result;
}
