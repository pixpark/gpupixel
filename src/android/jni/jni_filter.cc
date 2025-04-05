/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include <jni.h>
#include <list>
#include <string>
#include <vector>

#include "filter.h"
#include "jni_helpers.h"

using namespace gpupixel;

// Global filter list for managing filter lifecycle
std::list<std::shared_ptr<Filter>> filter_list_;

// Create filter
extern "C" jlong Java_com_pixpark_gpupixel_GPUPixelFilter_nativeFilterCreate(
    JNIEnv* env,
    jclass obj,
    jstring jFilterClassName) {
  const char* filterClassName = env->GetStringUTFChars(jFilterClassName, 0);

  auto ft = Filter::Create(filterClassName);
  filter_list_.push_back(ft);
  jlong ret = (jlong)ft.get();
  env->ReleaseStringUTFChars(jFilterClassName, filterClassName);
  return ret;
};

// Destroy filter
extern "C" void Java_com_pixpark_gpupixel_GPUPixelFilter_nativeFilterDestroy(
    JNIEnv* env,
    jclass obj,
    jlong classId) {
  for (auto ft : filter_list_) {
    if (classId == (jlong)ft.get()) {
      filter_list_.remove(ft);
    }
  }
};

// Release filter resources
extern "C" void Java_com_pixpark_gpupixel_GPUPixelFilter_nativeFilterFinalize(
    JNIEnv* env,
    jclass obj,
    jlong classId) {
  ((Filter*)classId)->ReleaseFramebuffer(false);
};

// Set float property for filter
extern "C" void
Java_com_pixpark_gpupixel_GPUPixelFilter_nativeFilterSetPropertyFloat(
    JNIEnv* env,
    jclass obj,
    jlong classId,
    jstring jProperty,
    jfloat value) {
  const char* property = env->GetStringUTFChars(jProperty, 0);
  ((Filter*)classId)->SetProperty(property, value);
  env->ReleaseStringUTFChars(jProperty, property);
};

// Set int property for filter
extern "C" void
Java_com_pixpark_gpupixel_GPUPixelFilter_nativeFilterSetPropertyInt(
    JNIEnv* env,
    jclass obj,
    jlong classId,
    jstring jProperty,
    jint value) {
  const char* property = env->GetStringUTFChars(jProperty, 0);
  ((Filter*)classId)->SetProperty(property, value);
  env->ReleaseStringUTFChars(jProperty, property);
};

// Set string property for filter
extern "C" void
Java_com_pixpark_gpupixel_GPUPixelFilter_nativeFilterSetPropertyString(
    JNIEnv* env,
    jclass obj,
    jlong classId,
    jstring jProperty,
    jstring jValue) {
  const char* property = env->GetStringUTFChars(jProperty, 0);
  const char* value = env->GetStringUTFChars(jValue, 0);
  ((Filter*)classId)->SetProperty(property, value);
  env->ReleaseStringUTFChars(jProperty, property);
  env->ReleaseStringUTFChars(jValue, value);
};

// Set float array property for filter
extern "C" JNIEXPORT void JNICALL
Java_com_pixpark_gpupixel_GPUPixelFilter_nativeFilterSetPropertyFloatArray(
    JNIEnv* env,
    jclass clazz,
    jlong class_id,
    jstring jProperty,
    jfloatArray jarray) {
  const char* property = env->GetStringUTFChars(jProperty, 0);
  jsize length = env->GetArrayLength(jarray);

  // Get pointer to the original Java array
  jfloat* c_array = env->GetFloatArrayElements(jarray, nullptr);
  std::vector<float> vector;
  for (int i = 0; i < length; i++) {
    vector.push_back(c_array[i]);
  }
  ((Filter*)class_id)->SetProperty(property, vector);
  env->ReleaseStringUTFChars(jProperty, property);
  // Release Java array memory
  env->ReleaseFloatArrayElements(jarray, c_array, JNI_ABORT);
}
