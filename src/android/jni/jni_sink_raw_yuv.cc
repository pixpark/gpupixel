#include <jni.h>
#include <list>
#include <string>

#include "android/jni/jni_helpers.h"
#include "core/gpupixel_context.h"
#include "gpupixel/sink/sink_raw_data.h"

using namespace gpupixel;

// Create a new SinkRawData instance
extern "C" JNIEXPORT jlong JNICALL
Java_com_pixpark_gpupixel_GPUPixelSinkRawYuv_nativeCreate(JNIEnv* env,
                                                           jclass clazz) {
  auto sink_raw_data = SinkRawYuv::Create();
  if (!sink_raw_data) {
    return 0;
  }

  // Create shared_ptr on heap
  auto* ptr = new std::shared_ptr<SinkRawYuv>(sink_raw_data);
  return reinterpret_cast<jlong>(ptr);
}

// Destroy a SinkRawData instance
extern "C" JNIEXPORT void JNICALL
Java_com_pixpark_gpupixel_GPUPixelSinkRawYuv_nativeDestroy(JNIEnv* env,
                                                            jclass clazz,
                                                            jlong native_obj) {
  // Free the heap-allocated shared_ptr
  auto* ptr = reinterpret_cast<std::shared_ptr<SinkRawYuv>*>(native_obj);
  delete ptr;
}

// Finalize SinkRawData resources
extern "C" JNIEXPORT void JNICALL
Java_com_pixpark_gpupixel_GPUPixelSinkRawYuv_nativeFinalize(JNIEnv* env,
                                                             jclass clazz,
                                                             jlong native_obj) {
  // Add cleanup code here if needed
  auto* ptr = reinterpret_cast<std::shared_ptr<SinkRawYuv>*>(native_obj);
  if (ptr && *ptr) {
    // Add special cleanup code here if needed
  }
}

// Get width
extern "C" JNIEXPORT jint JNICALL
Java_com_pixpark_gpupixel_GPUPixelSinkRawYuv_nativeGetWidth(JNIEnv* env,
                                                             jclass clazz,
                                                             jlong native_obj) {
  auto* ptr = reinterpret_cast<std::shared_ptr<SinkRawYuv>*>(native_obj);
  return ptr && *ptr ? (*ptr)->GetWidth() : 0;
}

// Get height
extern "C" JNIEXPORT jint JNICALL
Java_com_pixpark_gpupixel_GPUPixelSinkRawYuv_nativeGetHeight(
    JNIEnv* env,
    jclass clazz,
    jlong native_obj) {
  auto* ptr = reinterpret_cast<std::shared_ptr<SinkRawYuv>*>(native_obj);
  return ptr && *ptr ? (*ptr)->GetHeight() : 0;
}

// Get I420 buffer data
extern "C" JNIEXPORT jobject JNICALL
Java_com_pixpark_gpupixel_GPUPixelSinkRawYuv_nativeGetI420Buffer(
    JNIEnv* env,
    jclass clazz,
    jlong native_obj) {
  auto* ptr = reinterpret_cast<std::shared_ptr<SinkRawYuv>*>(native_obj);
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

  jobject result = env->NewDirectByteBuffer((void *) buffer, size);
  return result;
}
