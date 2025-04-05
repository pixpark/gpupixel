/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "jni_helpers.h"

#include <android/log.h>
#include <asm/unistd.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <unistd.h>

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
