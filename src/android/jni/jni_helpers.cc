/*
 *  Copyright 2015 The GPUPixel project authors. All Rights
 * Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#include "android/jni/jni_helpers.h"

#include <asm/unistd.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <unistd.h>

JavaVM* g_jvm = nullptr;
JavaVM* GetJVM() {
  return g_jvm;
}

void SetJVM(JavaVM* jvm) {
  g_jvm = jvm;
}

JNIEnv* GetEnv(JavaVM* jvm) {
  void* env = NULL;
  jint status = jvm->GetEnv(&env, JNI_VERSION_1_6);
  //    RTC_CHECK(((env != NULL) && (status == JNI_OK)) ||
  //              ((env == NULL) && (status == JNI_EDETACHED)))
  //            << "Unexpected GetEnv return: " << status << ":" << env;
  return reinterpret_cast<JNIEnv*>(env);
}

// Given a UTF-8 encoded |native| string return a new (UTF-16) jstring.
jstring JavaStringFromStdString(JNIEnv* jni, const std::string& native) {
  jstring jstr = jni->NewStringUTF(native.c_str());
  //    CHECK_EXCEPTION(jni) << "error during NewStringUTF";
  return jstr;
}

// Given a (UTF-16) jstring return a new UTF-8 native string.
std::string JavaToStdString(JNIEnv* jni, const jstring& j_string) {
  const char* chars = jni->GetStringUTFChars(j_string, nullptr);
  //    CHECK_EXCEPTION(jni) << "Error during GetStringUTFChars";
  std::string str(chars, jni->GetStringUTFLength(j_string));
  //    CHECK_EXCEPTION(jni) << "Error during GetStringUTFLength";
  jni->ReleaseStringUTFChars(j_string, chars);
  //    CHECK_EXCEPTION(jni) << "Error during ReleaseStringUTFChars";
  return str;
}

AttachThreadScoped::AttachThreadScoped(JavaVM* jvm)
    : attached_(false), jvm_(jvm), env_(NULL) {
  env_ = GetEnv(jvm);
  if (!env_) {
    // Adding debug log here so we can track down potential leaks and figure
    // out why we sometimes see "Native thread exiting without having called
    // DetachCurrentThread" in logcat outputs.
    //        ALOGD("Attaching thread to JVM%s", GetThreadInfo().c_str());
    jint res = jvm->AttachCurrentThread(&env_, NULL);
    attached_ = (res == JNI_OK);
    //        RTC_CHECK(attached_) << "AttachCurrentThread failed: " << res;
  }
}

AttachThreadScoped::~AttachThreadScoped() {
  if (attached_) {
    //        ALOGD("Detaching thread from JVM%s", GetThreadInfo().c_str());
    jint res = jvm_->DetachCurrentThread();
    //        RTC_CHECK(res == JNI_OK) << "DetachCurrentThread failed: " << res;
    //        RTC_CHECK(!GetEnv(jvm_));
  }
}

JNIEnv* AttachThreadScoped::env() {
  return env_;
}

char* Jstring2CStr(JNIEnv* env, jstring jstr) {
  char* rtn = NULL;
  jclass cls_string = env->FindClass("java/lang/String");
  jstring str_encode = env->NewStringUTF("GB2312");
  jmethodID mid =
      env->GetMethodID(cls_string, "getBytes", "(Ljava/lang/String;)[B");
  jbyteArray barr = (jbyteArray)env->CallObjectMethod(jstr, mid, str_encode);
  jsize alen = env->GetArrayLength(barr);
  jbyte* ba = env->GetByteArrayElements(barr, JNI_FALSE);
  if (alen > 0) {
    rtn = (char*)malloc(alen + 1);  // new char[alen+1];
    memcpy(rtn, ba, alen);
    rtn[alen] = 0;
  }
  env->ReleaseByteArrayElements(barr, ba, 0);
  env->DeleteLocalRef(str_encode);
  env->DeleteLocalRef(cls_string);
  return rtn;
}
