/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "android/jni/jni_helpers.h"

#include <android/log.h>
#include <asm/unistd.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <cstring>
#include <string>
#include "libyuv/convert.h"
#include "libyuv/convert_argb.h"
#include "libyuv/planar_functions.h"
#include "libyuv/rotate.h"

#define LOG_TAG "JNI_GPUPixel"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// Called when the SO library is loaded
// Gets the JavaVM pointer and sets it in jni_helpers.h
extern "C" jint JNI_OnLoad(JavaVM* vm, void* reserved) {
  JNIEnv* env = nullptr;

  // Get JNIEnv
  if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
    LOGE("JNI_OnLoad failed to get the environment");
    return JNI_ERR;
  }

  // Set JavaVM pointer using SetJVM function in jni_helpers.h
  LOGI("Setting JavaVM pointer in JNI_OnLoad");
  SetJVM(vm);

  // Return JNI version
  return JNI_VERSION_1_6;
}

/**
 * Convert YUV420 format to RGBA format
 */
extern "C" JNIEXPORT void JNICALL
Java_com_pixpark_gpupixel_GPUPixel_nativeYUV420ToRGBA(JNIEnv* env,
                                                      jclass clazz,
                                                      jobject y_buffer,
                                                      jobject u_buffer,
                                                      jobject v_buffer,
                                                      jint width,
                                                      jint height,
                                                      jint y_row_stride,
                                                      jint u_row_stride,
                                                      jint v_row_stride,
                                                      jint y_pixel_stride,
                                                      jint u_pixel_stride,
                                                      jint v_pixel_stride,
                                                      jbyteArray rgba_out) {
  // Get input buffers
  uint8_t* y_data = (uint8_t*)env->GetDirectBufferAddress(y_buffer);
  uint8_t* u_data = (uint8_t*)env->GetDirectBufferAddress(u_buffer);
  uint8_t* v_data = (uint8_t*)env->GetDirectBufferAddress(v_buffer);

  // Get output array
  jbyte* rgba_data = env->GetByteArrayElements(rgba_out, nullptr);

  if (!y_data || !u_data || !v_data || !rgba_data) {
    LOGE("Failed to get buffer addresses");
    if (rgba_data) {
      env->ReleaseByteArrayElements(rgba_out, rgba_data, 0);
    }
    return;
  }

  // If pixel stride is 1, use standard libyuv conversion function
  if (y_pixel_stride == 1 && u_pixel_stride == 1 && v_pixel_stride == 1) {
    // Create temporary NV12/NV21 buffer (Y plane and interleaved UV plane)
    uint8_t* nv21_data = new uint8_t[width * height * 3 / 2];
    if (!nv21_data) {
      LOGE("Memory allocation failed");
      env->ReleaseByteArrayElements(rgba_out, rgba_data, 0);
      return;
    }

    // Copy Y plane
    for (int i = 0; i < height; i++) {
      memcpy(nv21_data + i * width, y_data + i * y_row_stride, width);
    }

    // Build NV21 format (VU interleaved) UV plane
    uint8_t* nv21_vu_data = nv21_data + width * height;
    int uv_height = height / 2;
    int uv_width = width / 2;

    for (int i = 0; i < uv_height; i++) {
      for (int j = 0; j < uv_width; j++) {
        int offset = i * u_row_stride + j;
        nv21_vu_data[i * width + j * 2] = v_data[offset];      // V
        nv21_vu_data[i * width + j * 2 + 1] = u_data[offset];  // U
      }
    }

    // Use libyuv to convert NV21 to RGBA
    libyuv::NV21ToABGR(nv21_data,                   // Y
                       width,                       // Y stride
                       nv21_data + width * height,  // VU
                       width,                       // VU stride
                       (uint8_t*)rgba_data,         // RGBA
                       width * 4,                   // RGBA stride
                       width, height);

    delete[] nv21_data;
  } else {
    // Non-standard pixel stride, handle manually
    LOGI("Non-standard pixel stride: Y=%d U=%d V=%d", y_pixel_stride,
         u_pixel_stride, v_pixel_stride);

    // Create temporary buffers
    uint8_t* y_plane = new uint8_t[width * height];
    uint8_t* u_plane = new uint8_t[width * height / 4];
    uint8_t* v_plane = new uint8_t[width * height / 4];

    if (!y_plane || !u_plane || !v_plane) {
      LOGE("Memory allocation failed");
      delete[] y_plane;
      delete[] u_plane;
      delete[] v_plane;
      env->ReleaseByteArrayElements(rgba_out, rgba_data, 0);
      return;
    }

    // Extract continuous plane data from stride data
    for (int i = 0; i < height; i++) {
      for (int j = 0; j < width; j++) {
        y_plane[i * width + j] = y_data[i * y_row_stride + j * y_pixel_stride];
      }
    }

    int uv_height = height / 2;
    int uv_width = width / 2;

    for (int i = 0; i < uv_height; i++) {
      for (int j = 0; j < uv_width; j++) {
        u_plane[i * uv_width + j] =
            u_data[i * u_row_stride + j * u_pixel_stride];
        v_plane[i * uv_width + j] =
            v_data[i * v_row_stride + j * v_pixel_stride];
      }
    }

    // Use libyuv to convert I420(YUV420) to RGBA
    libyuv::I420ToABGR(y_plane,              // Y
                       width,                // Y stride
                       u_plane,              // U
                       uv_width,             // U stride
                       v_plane,              // V
                       uv_width,             // V stride
                       (uint8_t*)rgba_data,  // RGBA
                       width * 4,            // RGBA stride
                       width, height);

    delete[] y_plane;
    delete[] u_plane;
    delete[] v_plane;
  }

  // Release Java array
  env->ReleaseByteArrayElements(rgba_out, rgba_data, 0);
}

/**
 * Rotate RGBA format image
 */
extern "C" JNIEXPORT void JNICALL
Java_com_pixpark_gpupixel_GPUPixel_nativeRotateRGBA(JNIEnv* env,
                                                    jclass clazz,
                                                    jbyteArray rgba_in,
                                                    jint width,
                                                    jint height,
                                                    jbyteArray rgba_out,
                                                    jint out_width,
                                                    jint out_height,
                                                    jint rotation_degrees) {
  // Get input and output arrays
  jbyte* rgba_in_data = env->GetByteArrayElements(rgba_in, nullptr);
  jbyte* rgba_out_data = env->GetByteArrayElements(rgba_out, nullptr);

  if (!rgba_in_data || !rgba_out_data) {
    LOGE("Failed to get array elements");
    if (rgba_in_data) {
      env->ReleaseByteArrayElements(rgba_in, rgba_in_data, JNI_ABORT);
    }
    if (rgba_out_data) {
      env->ReleaseByteArrayElements(rgba_out, rgba_out_data, JNI_ABORT);
    }
    return;
  }

  // Rotate RGBA image using pure C++ code
  uint8_t* src = (uint8_t*)rgba_in_data;
  uint8_t* dst = (uint8_t*)rgba_out_data;

  // Process based on rotation angle
  switch (rotation_degrees) {
    case 0:  // No rotation
      memcpy(dst, src, width * height * 4);
      break;

    case 90:  // Rotate 90 degrees clockwise
      for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
          // Source pixel position in RGBA array
          int srcIdx = (y * width + x) * 4;

          // Target pixel position: (height - 1 - y, x) -> (x, height - 1 - y)
          // After 90° rotation, x coordinate becomes row(y), y coordinate
          // becomes column(x)
          int dstX = height - 1 - y;
          int dstY = x;
          int dstIdx = (dstY * out_width + dstX) * 4;

          // Copy all four RGBA channels
          dst[dstIdx] = src[srcIdx];          // R
          dst[dstIdx + 1] = src[srcIdx + 1];  // G
          dst[dstIdx + 2] = src[srcIdx + 2];  // B
          dst[dstIdx + 3] = src[srcIdx + 3];  // A
        }
      }
      break;

    case 180:  // Rotate 180 degrees
      for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
          // Source pixel position in RGBA array
          int srcIdx = (y * width + x) * 4;

          // Target pixel position: (width - 1 - x, height - 1 - y)
          int dstX = width - 1 - x;
          int dstY = height - 1 - y;
          int dstIdx = (dstY * out_width + dstX) * 4;

          // Copy all four RGBA channels
          dst[dstIdx] = src[srcIdx];          // R
          dst[dstIdx + 1] = src[srcIdx + 1];  // G
          dst[dstIdx + 2] = src[srcIdx + 2];  // B
          dst[dstIdx + 3] = src[srcIdx + 3];  // A
        }
      }
      break;

    case 270:  // Rotate 270 degrees clockwise (90 degrees counterclockwise)
      for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
          // Source pixel position in RGBA array
          int srcIdx = (y * width + x) * 4;

          // Target pixel position: (y, width - 1 - x)
          int dstX = y;
          int dstY = width - 1 - x;
          int dstIdx = (dstY * out_width + dstX) * 4;

          // Copy all four RGBA channels
          dst[dstIdx] = src[srcIdx];          // R
          dst[dstIdx + 1] = src[srcIdx + 1];  // G
          dst[dstIdx + 2] = src[srcIdx + 2];  // B
          dst[dstIdx + 3] = src[srcIdx + 3];  // A
        }
      }
      break;

    default:
      LOGE("Unsupported rotation angle: %d", rotation_degrees);
      memcpy(dst, src, width * height * 4);
      break;
  }

  // Release Java arrays
  env->ReleaseByteArrayElements(rgba_in, rgba_in_data, JNI_ABORT);
  env->ReleaseByteArrayElements(rgba_out, rgba_out_data, 0);
}
