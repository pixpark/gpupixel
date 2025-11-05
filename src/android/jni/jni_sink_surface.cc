/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

/**
 * @file jni_sink_surface.cc
 * @brief JNI bridge implementation for SinkSurface
 *
 * Provides interface between Java layer GPUPixelSinkSurface class and C++ layer SinkSurface class.
 * Main functions:
 * - Object creation and destruction
 * - Conversion between Android Surface and ANativeWindow
 * - Setting fill mode and mirror mode
 *
 * JNI naming convention: Java_PackageName_ClassName_MethodName
 */

#if defined(__ANDROID__) || defined(GPUPIXEL_ANDROID)

#include <jni.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>

#include "android/jni/jni_helpers.h"
#include "core/gpupixel_context.h"
#include "gpupixel/sink/sink_surface.h"

using namespace gpupixel;


/**
 * @brief Create SinkSurface instance (JNI)
 * @param env JNI environment
 * @param clazz Java class object
 * @return C++ object pointer (returned as jlong)
 * 
 * Corresponds to Java method: GPUPixelSinkSurface.nativeCreate()
 */
extern "C" JNIEXPORT jlong JNICALL
Java_com_pixpark_gpupixel_GPUPixelSinkSurface_nativeCreate(JNIEnv* env,
                                                           jclass clazz) {
  auto sink_surface = SinkSurface::Create();
  if (!sink_surface) {
    return 0;
  }

  // Create shared_ptr on heap
  auto* ptr = new std::shared_ptr<SinkSurface>(sink_surface);
  return reinterpret_cast<jlong>(ptr);
}

/**
 * @brief Destroy SinkSurface instance (JNI)
 * @param env JNI environment
 * @param clazz Java class object
 * @param native_obj C++ object pointer (jlong)
 * 
 * Corresponds to Java method: GPUPixelSinkSurface.nativeDestroy()
 * Release the shared_ptr pointer allocated on heap
 */
extern "C" JNIEXPORT void JNICALL
Java_com_pixpark_gpupixel_GPUPixelSinkSurface_nativeDestroy(JNIEnv* env,
                                                            jclass clazz,
                                                            jlong native_obj) {
  // 释放堆上分配的shared_ptr
  auto* ptr = reinterpret_cast<std::shared_ptr<SinkSurface>*>(native_obj);
  delete ptr;
}

/**
 * @brief Finalize SinkSurface resources (JNI)
 * @param env JNI environment
 * @param clazz Java class object
 * @param native_obj C++ object pointer (jlong)
 * 
 * Corresponds to Java method: GPUPixelSinkSurface.nativeFinalize()
 * Called in Java object's finalize() method, release native window resources
 */
extern "C" JNIEXPORT void JNICALL
Java_com_pixpark_gpupixel_GPUPixelSinkSurface_nativeFinalize(JNIEnv* env,
                                                            jclass clazz,
                                                            jlong native_obj) {
  auto* ptr = reinterpret_cast<std::shared_ptr<SinkSurface>*>(native_obj);
  if (ptr && *ptr) {
    // 释放原生窗口资源（会销毁EGL Surface）
    (*ptr)->ReleaseNativeWindow();
  }
}

/**
 * @brief Set Android Surface for rendering (JNI)
 * @param env JNI environment
 * @param clazz Java class object
 * @param native_obj C++ object pointer (jlong)
 * @param surface Android Surface object
 * @param width Surface width (in pixels)
 * @param height Surface height (in pixels)
 * 
 * Corresponds to Java method: GPUPixelSinkSurface.nativeSetSurface()
 * Get ANativeWindow from Android Surface, and create EGL Window Surface
 */
extern "C" JNIEXPORT void JNICALL
Java_com_pixpark_gpupixel_GPUPixelSinkSurface_nativeSetSurface(
    JNIEnv* env, jclass clazz, jlong native_obj, jobject surface, jint width,
    jint height) {
  auto* ptr = reinterpret_cast<std::shared_ptr<SinkSurface>*>(native_obj);
  if (!ptr || !*ptr) {
    return;
  }

  // If Surface is null, release native window
  if (surface == nullptr) {
    (*ptr)->ReleaseNativeWindow();
    return;
  }

  // Get ANativeWindow from Android Surface
  // This is the key conversion in JNI layer, converting Java's Surface to C layer's ANativeWindow
  ANativeWindow* native_window = ANativeWindow_fromSurface(env, surface);
  if (native_window == nullptr) {
    return;
  }

  // Set native window, will create EGL Window Surface
  (*ptr)->SetNativeWindow(native_window, width, height);

  // Note: Don't release native_window here, as it's needed for rendering
  // native_window will be released in ReleaseNativeWindow()
}

/**
 * @brief Release Surface (JNI)
 * @param env JNI environment
 * @param clazz Java class object
 * @param native_obj C++ object pointer (jlong)
 * 
 * Corresponds to Java method: GPUPixelSinkSurface.nativeReleaseSurface()
 * Release native window resources, destroy EGL Surface
 */
extern "C" JNIEXPORT void JNICALL
Java_com_pixpark_gpupixel_GPUPixelSinkSurface_nativeReleaseSurface(
    JNIEnv* env, jclass clazz, jlong native_obj) {
  auto* ptr = reinterpret_cast<std::shared_ptr<SinkSurface>*>(native_obj);
  if (ptr && *ptr) {
    // 释放原生窗口，会同时销毁EGL Surface
    (*ptr)->ReleaseNativeWindow();
  }
}

/**
 * @brief Set fill mode (JNI)
 * @param env JNI environment
 * @param clazz Java class object
 * @param native_obj C++ object pointer (jlong)
 * @param fill_mode Fill mode (0=stretch, 1=preserve aspect ratio, 2=preserve aspect ratio and fill)
 * 
 * Corresponds to Java method: GPUPixelSinkSurface.nativeSetFillMode()
 */
extern "C" JNIEXPORT void JNICALL
Java_com_pixpark_gpupixel_GPUPixelSinkSurface_nativeSetFillMode(
    JNIEnv* env, jclass clazz, jlong native_obj, jint fill_mode) {
  auto* ptr = reinterpret_cast<std::shared_ptr<SinkSurface>*>(native_obj);
  if (ptr && *ptr) {
    // 将Java的int转换为C++的FillMode枚举
    SinkSurface::FillMode mode =
        static_cast<SinkSurface::FillMode>(fill_mode);
    (*ptr)->SetFillMode(mode);
  }
}

/**
 * @brief Set mirror mode (JNI)
 * @param env JNI environment
 * @param clazz Java class object
 * @param native_obj C++ object pointer (jlong)
 * @param mirror Whether to mirror (JNI_TRUE means horizontal mirror, JNI_FALSE means normal display)
 * 
 * Corresponds to Java method: GPUPixelSinkSurface.nativeSetMirror()
 */
extern "C" JNIEXPORT void JNICALL
Java_com_pixpark_gpupixel_GPUPixelSinkSurface_nativeSetMirror(
    JNIEnv* env, jclass clazz, jlong native_obj, jboolean mirror) {
  auto* ptr = reinterpret_cast<std::shared_ptr<SinkSurface>*>(native_obj);
  if (ptr && *ptr) {
    // 将JNI的jboolean转换为C++的bool
    (*ptr)->SetMirror(mirror == JNI_TRUE);
  }
}

#endif  // __ANDROID__ || GPUPIXEL_ANDROID