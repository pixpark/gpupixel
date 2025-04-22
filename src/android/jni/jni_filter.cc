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

#include "android/jni/jni_helpers.h"
#include "gpupixel/filter/filter.h"

using namespace gpupixel;

// Create filter
extern "C" JNIEXPORT jlong JNICALL
Java_com_pixpark_gpupixel_GPUPixelFilter_nativeFilterCreate(
    JNIEnv* env,
    jclass obj,
    jstring jFilterClassName) {
  const char* filterClassName = env->GetStringUTFChars(jFilterClassName, 0);

  auto filter = Filter::Create(filterClassName);
  env->ReleaseStringUTFChars(jFilterClassName, filterClassName);

  if (!filter) {
    return 0;
  }

  // Create shared_ptr on heap
  auto* ptr = new std::shared_ptr<Filter>(filter);
  return reinterpret_cast<jlong>(ptr);
};

// Destroy filter
extern "C" JNIEXPORT void JNICALL
Java_com_pixpark_gpupixel_GPUPixelFilter_nativeFilterDestroy(JNIEnv* env,
                                                             jclass obj,
                                                             jlong classId) {
  // Free the heap-allocated shared_ptr
  auto* ptr = reinterpret_cast<std::shared_ptr<Filter>*>(classId);
  delete ptr;
};

// Release filter resources
extern "C" JNIEXPORT void JNICALL
Java_com_pixpark_gpupixel_GPUPixelFilter_nativeFilterFinalize(JNIEnv* env,
                                                              jclass obj,
                                                              jlong classId) {
  // Get shared_ptr and access the object
  auto* ptr = reinterpret_cast<std::shared_ptr<Filter>*>(classId);
  if (ptr && *ptr) {
    (*ptr)->ReleaseFramebuffer(false);
  }
};

// Set float property for filter
extern "C" JNIEXPORT void JNICALL
Java_com_pixpark_gpupixel_GPUPixelFilter_nativeFilterSetPropertyFloat(
    JNIEnv* env,
    jclass obj,
    jlong classId,
    jstring jProperty,
    jfloat value) {
  auto* ptr = reinterpret_cast<std::shared_ptr<Filter>*>(classId);
  if (!ptr || !*ptr) {
    return;
  }

  const char* property = env->GetStringUTFChars(jProperty, 0);
  (*ptr)->SetProperty(property, value);
  env->ReleaseStringUTFChars(jProperty, property);
};

// Set int property for filter
extern "C" JNIEXPORT void JNICALL
Java_com_pixpark_gpupixel_GPUPixelFilter_nativeFilterSetPropertyInt(
    JNIEnv* env,
    jclass obj,
    jlong classId,
    jstring jProperty,
    jint value) {
  auto* ptr = reinterpret_cast<std::shared_ptr<Filter>*>(classId);
  if (!ptr || !*ptr) {
    return;
  }

  const char* property = env->GetStringUTFChars(jProperty, 0);
  (*ptr)->SetProperty(property, value);
  env->ReleaseStringUTFChars(jProperty, property);
};

// Set string property for filter
extern "C" JNIEXPORT void JNICALL
Java_com_pixpark_gpupixel_GPUPixelFilter_nativeFilterSetPropertyString(
    JNIEnv* env,
    jclass obj,
    jlong classId,
    jstring jProperty,
    jstring jValue) {
  auto* ptr = reinterpret_cast<std::shared_ptr<Filter>*>(classId);
  if (!ptr || !*ptr) {
    return;
  }

  const char* property = env->GetStringUTFChars(jProperty, 0);
  const char* value = env->GetStringUTFChars(jValue, 0);
  (*ptr)->SetProperty(property, value);
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
  auto* ptr = reinterpret_cast<std::shared_ptr<Filter>*>(class_id);
  if (!ptr || !*ptr) {
    return;
  }

  const char* property = env->GetStringUTFChars(jProperty, 0);
  jsize length = env->GetArrayLength(jarray);

  // Get pointer to the original Java array
  jfloat* c_array = env->GetFloatArrayElements(jarray, nullptr);
  std::vector<float> vector;
  for (int i = 0; i < length; i++) {
    vector.push_back(c_array[i]);
  }

  (*ptr)->SetProperty(property, vector);

  env->ReleaseStringUTFChars(jProperty, property);
  // Release Java array memory
  env->ReleaseFloatArrayElements(jarray, c_array, JNI_ABORT);
}
