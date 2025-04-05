#include <jni.h>
#include <list>
#include <string>

#include "filter.h"
#include "jni_helpers.h"
#include "sink.h"
#include "source.h"

using namespace gpupixel;

// Add Source to Sink connection
extern "C" JNIEXPORT jlong JNICALL
Java_com_pixpark_gpupixel_GPUPixelSource_nativeAddSink(JNIEnv* env,
                                                       jclass clazz,
                                                       jlong source_id,
                                                       jlong sink_id,
                                                       jint tex_id,
                                                       jboolean is_filter) {
  Source* source = (Source*)source_id;
  std::shared_ptr<Sink> sink = is_filter
                                   ? std::shared_ptr<Sink>((Filter*)sink_id)
                                   : std::shared_ptr<Sink>((Sink*)sink_id);
  if (tex_id >= 0) {
    return (jlong)(source->AddSink(sink, tex_id)).get();
  } else {
    return (jlong)(source->AddSink(sink)).get();
  }
}

// Remove Sink from Source
extern "C" JNIEXPORT void JNICALL
Java_com_pixpark_gpupixel_GPUPixelSource_nativeRemoveSink(JNIEnv* env,
                                                          jclass clazz,
                                                          jlong source_id,
                                                          jlong sink_id,
                                                          jboolean is_filter) {
  Source* source = (Source*)source_id;
  Sink* sink =
      is_filter ? dynamic_cast<Sink*>((Filter*)sink_id) : (Sink*)sink_id;
  source->RemoveSink(std::shared_ptr<Sink>(sink));
}

// Remove all Sinks from Source
extern "C" JNIEXPORT void JNICALL
Java_com_pixpark_gpupixel_GPUPixelSource_nativeRemoveAllSinks(JNIEnv* env,
                                                              jclass clazz,
                                                              jlong source_id) {
  ((Source*)source_id)->RemoveAllSinks();
}

// Execute rendering
extern "C" JNIEXPORT jboolean JNICALL
Java_com_pixpark_gpupixel_GPUPixelSource_nativeProceed(JNIEnv* env,
                                                       jclass clazz,
                                                       jlong source_id,
                                                       jboolean update_sinks) {
  return ((Source*)source_id)->DoRender(update_sinks);
}

// Get rotated framebuffer width
extern "C" JNIEXPORT jint JNICALL
Java_com_pixpark_gpupixel_GPUPixelSource_nativeGetRotatedFramebufferWidth(
    JNIEnv* env,
    jclass clazz,
    jlong source_id) {
  return ((Source*)source_id)->GetRotatedFramebufferWidth();
}

// Get rotated framebuffer height
extern "C" JNIEXPORT jint JNICALL
Java_com_pixpark_gpupixel_GPUPixelSource_nativeGetRotatedFramebufferHeight(
    JNIEnv* env,
    jclass clazz,
    jlong source_id) {
  return ((Source*)source_id)->GetRotatedFramebufferHeight();
}

// Get processed frame data
extern "C" JNIEXPORT jbyteArray JNICALL
Java_com_pixpark_gpupixel_GPUPixelSource_nativeGetProcessedFrameData(
    JNIEnv* env,
    jclass clazz,
    jlong source_id,
    jlong up_to_filter_id,
    jint width,
    jint height) {
  unsigned char* processed_frame_data =
      ((Source*)source_id)
          ->GetProcessedFrameData(
              std::shared_ptr<Filter>((Filter*)up_to_filter_id), width, height);

  int frame_size = width * height * 4 * sizeof(unsigned char);

  jbyteArray result = NULL;
  if (processed_frame_data) {
    jbyte* by = (jbyte*)processed_frame_data;
    result = env->NewByteArray(frame_size);
    env->SetByteArrayRegion(result, 0, frame_size, by);
    delete[] processed_frame_data;
    processed_frame_data = 0;
  }

  return result;
}
