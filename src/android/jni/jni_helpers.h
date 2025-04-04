/*
 *  Copyright 2015 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

// This file contain convenience functions and classes for JNI.
// Before using any of the methods, InitGlobalJniVariables must be called.

#ifndef WEBRTC_API_JAVA_JNI_JNI_HELPERS_H_
#define WEBRTC_API_JAVA_JNI_JNI_HELPERS_H_

#include <jni.h>

#include <string>

// Given a UTF-8 encoded |native| string return a new (UTF-16) jstring.
jstring JavaStringFromStdString(JNIEnv* jni, const std::string& native);

// Given a (UTF-16) jstring return a new UTF-8 native string.
std::string JavaToStdString(JNIEnv* jni, const jstring& j_string);

JavaVM* GetJVM();
void SetJVM(JavaVM* jvm);

JNIEnv* GetEnv(JavaVM* jvm);

// Attach thread to JVM if necessary and detach at scope end if originally
// attached.
class AttachThreadScoped {
 public:
  explicit AttachThreadScoped(JavaVM* jvm);
  ~AttachThreadScoped();
  JNIEnv* env();

 private:
  bool attached_;
  JavaVM* jvm_;
  JNIEnv* env_;
};

char* Jstring2CStr(JNIEnv* env, jstring jstr);
#endif  // WEBRTC_API_JAVA_JNI_JNI_HELPERS_H_
