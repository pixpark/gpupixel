#include <jni.h>
#include <list>
#include <string>

#include "gpupixel_context.h"
#include "jni_helpers.h"
#include "sink_raw_data.h"

using namespace gpupixel;

// Maintain all created SinkRawData objects
static std::list<std::shared_ptr<SinkRawData>> sink_raw_data_list_;

// Create a new SinkRawData instance
extern "C" JNIEXPORT jlong JNICALL
Java_com_pixpark_gpupixel_GPUPixelSinkRawData_nativeCreate(JNIEnv* env,
                                                           jclass clazz) {
  auto sink_raw_data = SinkRawData::Create();
  if (!sink_raw_data) {
    return 0;
  }
  sink_raw_data_list_.push_back(sink_raw_data);
  return (jlong)(sink_raw_data.get());
}

// Destroy a SinkRawData instance
extern "C" JNIEXPORT void JNICALL
Java_com_pixpark_gpupixel_GPUPixelSinkRawData_nativeDestroy(JNIEnv* env,
                                                            jclass clazz,
                                                            jlong native_obj) {
  for (auto srd : sink_raw_data_list_) {
    if ((jlong)srd.get() == native_obj) {
      sink_raw_data_list_.remove(srd);
      break;
    }
  }
}

// Finalize SinkRawData resources
extern "C" JNIEXPORT void JNICALL
Java_com_pixpark_gpupixel_GPUPixelSinkRawData_nativeFinalize(JNIEnv* env,
                                                             jclass clazz,
                                                             jlong native_obj) {
  // May need to perform some cleanup
}

// Execute rendering
extern "C" JNIEXPORT void JNICALL
Java_com_pixpark_gpupixel_GPUPixelSinkRawData_nativeRender(JNIEnv* env,
                                                           jclass clazz,
                                                           jlong native_obj) {
  ((SinkRawData*)native_obj)->Render();
}

// Get width
extern "C" JNIEXPORT jint JNICALL
Java_com_pixpark_gpupixel_GPUPixelSinkRawData_nativeGetWidth(JNIEnv* env,
                                                             jclass clazz,
                                                             jlong native_obj) {
  return ((SinkRawData*)native_obj)->GetWidth();
}

// Get height
extern "C" JNIEXPORT jint JNICALL
Java_com_pixpark_gpupixel_GPUPixelSinkRawData_nativeGetHeight(
    JNIEnv* env,
    jclass clazz,
    jlong native_obj) {
  return ((SinkRawData*)native_obj)->GetHeight();
}

// Get RGBA buffer data
extern "C" JNIEXPORT jbyteArray JNICALL
Java_com_pixpark_gpupixel_GPUPixelSinkRawData_nativeGetRgbaBuffer(
    JNIEnv* env,
    jclass clazz,
    jlong native_obj) {
  const uint8_t* buffer = ((SinkRawData*)native_obj)->GetRgbaBuffer();
  if (!buffer) {
    return NULL;
  }

  int width = ((SinkRawData*)native_obj)->GetWidth();
  int height = ((SinkRawData*)native_obj)->GetHeight();
  int size = width * height * 4;  // RGBA = 4 bytes per pixel

  jbyteArray result = env->NewByteArray(size);
  env->SetByteArrayRegion(result, 0, size, (jbyte*)buffer);

  return result;
}

// Get I420 buffer data
extern "C" JNIEXPORT jbyteArray JNICALL
Java_com_pixpark_gpupixel_GPUPixelSinkRawData_nativeGetI420Buffer(
    JNIEnv* env,
    jclass clazz,
    jlong native_obj) {
  const uint8_t* buffer = ((SinkRawData*)native_obj)->GetI420Buffer();
  if (!buffer) {
    return NULL;
  }

  int width = ((SinkRawData*)native_obj)->GetWidth();
  int height = ((SinkRawData*)native_obj)->GetHeight();
  int size = width * height * 3 / 2;  // I420 size = width * height * 1.5

  jbyteArray result = env->NewByteArray(size);
  env->SetByteArrayRegion(result, 0, size, (jbyte*)buffer);

  return result;
}
