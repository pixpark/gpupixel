#include <android/log.h>
#include <jni.h>
#include <list>
#include <string>

#include "gpupixel/filter/filter.h"
#include "gpupixel/sink/sink.h"
#include "gpupixel/source/source.h"
#include "utils/util.h"

using namespace gpupixel;

// Define log tag
#define LOG_TAG "GPUPixel_Source_JNI"

// Add Source to Sink connection
extern "C" JNIEXPORT jlong JNICALL
Java_com_pixpark_gpupixel_GPUPixelSource_nativeAddSink(JNIEnv* env,
                                                       jclass clazz,
                                                       jlong source_id,
                                                       jlong sink_id,
                                                       jboolean is_sink_filter,
                                                       jboolean is_source_filter) {
  if (source_id == 0) {
    Util::Log(LOG_TAG, "nativeAddSink - Invalid parameter: source_id is 0");
    return 0;
  }

  if (sink_id == 0) {
    Util::Log(LOG_TAG, "nativeAddSink - Invalid parameter: sink_id is 0");
    return 0;
  }

  // Get Source pointer based on is_source_filter parameter
  std::shared_ptr<Source> source;
  
  if (is_source_filter) {
    // If source is a Filter type
    auto* filter_ptr = reinterpret_cast<std::shared_ptr<Filter>*>(source_id);
    if (!filter_ptr) {
      Util::Log(LOG_TAG, "nativeAddSink - source filter_ptr is null, conversion failed");
      return 0;
    }

    if (!*filter_ptr) {
      Util::Log(LOG_TAG, "nativeAddSink - *source filter_ptr is null, invalid Filter object");
      return 0;
    }

    source = std::static_pointer_cast<Source>(*filter_ptr);
  } else {
    // If it's a regular Source type
    auto* source_ptr = reinterpret_cast<std::shared_ptr<Source>*>(source_id);
    if (!source_ptr) {
      Util::Log(LOG_TAG, "nativeAddSink - source_ptr is null, conversion failed");
      return 0;
    }

    if (!*source_ptr) {
      Util::Log(LOG_TAG,
                "nativeAddSink - *source_ptr is null, invalid Source object");
      return 0;
    }
    
    source = *source_ptr;
  }

  std::shared_ptr<Sink> sink;

  // Process sink_id based on is_sink_filter parameter
  if (is_sink_filter) {
    // If it's a Filter type
    auto* filter_ptr = reinterpret_cast<std::shared_ptr<Filter>*>(sink_id);
    if (!filter_ptr) {
      Util::Log(LOG_TAG,
                "nativeAddSink - sink filter_ptr is null, conversion failed");
      return 0;
    }

    if (!*filter_ptr) {
      Util::Log(LOG_TAG,
                "nativeAddSink - *sink filter_ptr is null, invalid Filter object");
      return 0;
    }

    sink = std::static_pointer_cast<Sink>(*filter_ptr);
  } else {
    // If it's a regular Sink type
    auto* sink_ptr = reinterpret_cast<std::shared_ptr<Sink>*>(sink_id);
    if (!sink_ptr) {
      Util::Log(LOG_TAG, "nativeAddSink - sink_ptr is null, conversion failed");
      return 0;
    }

    if (!*sink_ptr) {
      Util::Log(LOG_TAG,
                "nativeAddSink - *sink_ptr is null, invalid Sink object");
      return 0;
    }

    sink = *sink_ptr;
  }

  // Add connection
  source->AddSink(sink);

  // Return the original sink_id
  return sink_id;
}

// Remove Sink from Source
extern "C" JNIEXPORT void JNICALL
Java_com_pixpark_gpupixel_GPUPixelSource_nativeRemoveSink(JNIEnv* env,
                                                          jclass clazz,
                                                          jlong source_id,
                                                          jlong sink_id,
                                                          jboolean is_sink_filter,
                                                          jboolean is_source_filter) {
  // Get Source pointer based on is_source_filter parameter
  std::shared_ptr<Source> source;
  
  if (is_source_filter) {
    // If source is a Filter type
    auto* filter_ptr = reinterpret_cast<std::shared_ptr<Filter>*>(source_id);
    if (!filter_ptr || !*filter_ptr) {
      Util::Log(LOG_TAG, "nativeRemoveSink - Invalid Source Filter object");
      return;
    }
    source = std::static_pointer_cast<Source>(*filter_ptr);
  } else {
    // If it's a regular Source type
    auto* source_ptr = reinterpret_cast<std::shared_ptr<Source>*>(source_id);
    if (!source_ptr || !*source_ptr) {
      Util::Log(LOG_TAG, "nativeRemoveSink - Invalid Source object");
      return;
    }
    source = *source_ptr;
  }

  std::shared_ptr<Sink> sink;

  // Process sink_id based on is_sink_filter parameter
  if (is_sink_filter) {
    // If it's a Filter type
    auto* filter_ptr = reinterpret_cast<std::shared_ptr<Filter>*>(sink_id);
    if (!filter_ptr || !*filter_ptr) {
      Util::Log(LOG_TAG, "nativeRemoveSink - Invalid Filter object");
      return;
    }

    sink = std::static_pointer_cast<Sink>(*filter_ptr);
  } else {
    // If it's a regular Sink type
    auto* sink_ptr = reinterpret_cast<std::shared_ptr<Sink>*>(sink_id);
    if (!sink_ptr || !*sink_ptr) {
      Util::Log(LOG_TAG, "nativeRemoveSink - Invalid Sink object");
      return;
    }

    sink = *sink_ptr;
  }

  // Remove connection
  source->RemoveSink(sink);
}

// Remove all Sinks from Source
extern "C" JNIEXPORT void JNICALL
Java_com_pixpark_gpupixel_GPUPixelSource_nativeRemoveAllSinks(JNIEnv* env,
                                                              jclass clazz,
                                                              jlong source_id) {
  auto* source_ptr = reinterpret_cast<std::shared_ptr<Source>*>(source_id);
  if (source_ptr && *source_ptr) {
    (*source_ptr)->RemoveAllSinks();
  }
}

// Check if Source has Sink
extern "C" JNIEXPORT jboolean JNICALL
Java_com_pixpark_gpupixel_GPUPixelSource_nativeHasSink(JNIEnv* env,
                                                       jclass clazz,
                                                       jlong source_id,
                                                       jlong sink_id,
                                                       jboolean is_sink_filter,
                                                       jboolean is_source_filter) {
  // Get Source pointer based on is_source_filter parameter
  std::shared_ptr<Source> source;
  
  if (is_source_filter) {
    // If source is a Filter type
    auto* filter_ptr = reinterpret_cast<std::shared_ptr<Filter>*>(source_id);
    if (!filter_ptr || !*filter_ptr) {
      return false;
    }
    source = std::static_pointer_cast<Source>(*filter_ptr);
  } else {
    // If it's a regular Source type
    auto* source_ptr = reinterpret_cast<std::shared_ptr<Source>*>(source_id);
    if (!source_ptr || !*source_ptr) {
      return false;
    }
    source = *source_ptr;
  }

  // Get Sink pointer based on is_sink_filter parameter
  std::shared_ptr<Sink> sink;
  
  if (is_sink_filter) {
    // If it's a Filter type
    auto* filter_ptr = reinterpret_cast<std::shared_ptr<Filter>*>(sink_id);
    if (!filter_ptr || !*filter_ptr) {
      return false;
    }
    sink = std::static_pointer_cast<Sink>(*filter_ptr);
  } else {
    // If it's a regular Sink type
    auto* sink_ptr = reinterpret_cast<std::shared_ptr<Sink>*>(sink_id);
    if (!sink_ptr || !*sink_ptr) {
      return false;
    }
    sink = *sink_ptr;
  }

  // Check connection
  return source->HasSink(sink);
}

// Set framebuffer scale
extern "C" JNIEXPORT void JNICALL
Java_com_pixpark_gpupixel_GPUPixelSource_nativeSetFramebufferScale(
    JNIEnv* env,
    jclass clazz,
    jlong source_id,
    jfloat scale) {
  auto* source_ptr = reinterpret_cast<std::shared_ptr<Source>*>(source_id);
  if (source_ptr && *source_ptr) {
    (*source_ptr)->SetFramebufferScale(scale);
  }
}

// Get rotated framebuffer width
extern "C" JNIEXPORT jint JNICALL
Java_com_pixpark_gpupixel_GPUPixelSource_nativeGetRotatedFramebufferWidth(
    JNIEnv* env,
    jclass clazz,
    jlong source_id) {
  auto* source_ptr = reinterpret_cast<std::shared_ptr<Source>*>(source_id);
  return source_ptr && *source_ptr ? (*source_ptr)->GetRotatedFramebufferWidth()
                                   : 0;
}

// Get rotated framebuffer height
extern "C" JNIEXPORT jint JNICALL
Java_com_pixpark_gpupixel_GPUPixelSource_nativeGetRotatedFramebufferHeight(
    JNIEnv* env,
    jclass clazz,
    jlong source_id) {
  auto* source_ptr = reinterpret_cast<std::shared_ptr<Source>*>(source_id);
  return source_ptr && *source_ptr
             ? (*source_ptr)->GetRotatedFramebufferHeight()
             : 0;
}
