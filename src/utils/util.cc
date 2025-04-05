/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "util.h"
#include <cstdarg>
#include "../core/gpupixel_context.h"
#if defined(GPUPIXEL_ANDROID)
#include <android/log.h>
#include "../android/jni/jni_helpers.h"
#elif defined(GPUPIXEL_IOS)
#import <Foundation/foundation.h>
#import <UIKit/UIKit.h>
#elif defined(GPUPIXEL_WIN)
#include <malloc.h>
#include <stdio.h>
#endif
#include <chrono>

#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_MAC)
@interface GPXObjcHelper : NSObject
+ (NSString*)getResourcePath:(NSString*)name;
+ (NSString*)getBundleResourceWithName:(NSString*)bundleName
                              fileName:(NSString*)fileName
                                  type:(NSString*)fileType;
@end

@implementation GPXObjcHelper

+ (NSString*)getResourcePath:(NSString*)name {
  NSString* path = [[[NSBundle bundleForClass:self.class] resourcePath]
      stringByAppendingPathComponent:name];
  return path;
}

+ (NSString*)getModelsPath {
  NSString* path = [[NSBundle bundleForClass:self.class] resourcePath];
  return path;
}

+ (NSString*)getBundleResourceWithName:(NSString*)bundleName
                              fileName:(NSString*)fileName
                                  type:(NSString*)fileType {
  NSBundle* sdkMainBundle = [NSBundle bundleForClass:[self class]];
  NSString* resourceBundlePath =
      [sdkMainBundle pathForResource:bundleName ofType:@"bundle"];
  NSBundle* resourceBundle = [NSBundle bundleWithPath:resourceBundlePath];
  NSString* filePath =
      [resourceBundle pathForResource:fileName ofType:fileType];
  return filePath;
}

@end

#endif

namespace gpupixel {

std::string Util::resourceRoot = "";

std::string Util::getResourcePath(std::string name) {
#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_MAC)
  NSString* oc_path = [GPXObjcHelper
      getResourcePath:[[NSString alloc] initWithUTF8String:name.c_str()]];
  std::string path = [oc_path UTF8String];
#elif defined(GPUPIXEL_ANDROID)
  std::string path = getResourcePathJni(name);
#else
  std::string path = resourceRoot.empty() ? name : (resourceRoot + "/" + name);
#endif
  return path;
}

void Util::setResourceRoot(std::string root) {
  resourceRoot = root;
}
#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_MAC)
std::string Util::getResourcePath(std::string bundle_name,
                                  std::string file_name,
                                  std::string type) {
  NSString* oc_path = [GPXObjcHelper
      getBundleResourceWithName:[[NSString alloc]
                                    initWithUTF8String:bundle_name.c_str()]
                       fileName:[[NSString alloc]
                                    initWithUTF8String:file_name.c_str()]
                           type:[[NSString alloc]
                                    initWithUTF8String:type.c_str()]];
  std::string path = [oc_path UTF8String];
  return path;
}

#endif

#ifdef GPUPIXEL_WIN
int vasprintf(char** strp, const char* fmt, va_list ap) {
  // _vscprintf tells you how big the buffer needs to be
  int len = _vscprintf(fmt, ap);
  if (len == -1) {
    return -1;
  }
  size_t size = (size_t)len + 1;
  char* str = (char*)malloc(size);
  if (!str) {
    return -1;
  }
  int r = _vsprintf_s_l(str, len + 1, fmt, nullptr, ap);
  if (r == -1) {
    free(str);
    return -1;
  }
  *strp = str;
  return r;
}
#endif

#if defined(GPUPIXEL_ANDROID)
std::string Util::getResourcePathJni(std::string name) {
  // Get JVM and environment
  JavaVM* jvm = GetJVM();
  if (!jvm) {
    Log("GPUPixel", "GetJVM() returned null in getResourcePathJni");
    return getDefaultResourcePath(name);
  }

  // Create JNI attach scope to ensure thread is attached to JVM
  AttachThreadScoped ats(jvm);
  JNIEnv* env = ats.env();
  if (!env) {
    Log("GPUPixel", "Failed to get JNIEnv in getResourcePathJni");
    return getDefaultResourcePath(name);
  }

  std::string result = "";

  // Find GPUPixel class - use fully qualified name
  jclass gpuPixelClass = nullptr;

  // Try different class paths
  const char* classNames[] = {
      "com/pixpark/gpupixel/GPUPixel",
  };

  for (const auto& className : classNames) {
    Log("GPUPixel", "Trying to find class: %s", className);
    gpuPixelClass = env->FindClass(className);
    if (gpuPixelClass) {
      Log("GPUPixel", "Found GPUPixel class: %s", className);
      break;
    }
    if (env->ExceptionCheck()) {
      env->ExceptionClear();
    }
  }

  if (!gpuPixelClass) {
    Log("GPUPixel",
        "Failed to find GPUPixel class after trying all alternatives");
    return getDefaultResourcePath(name);
  }

  // Get init static method to ensure initialization has been called
  jmethodID initMethod = env->GetStaticMethodID(gpuPixelClass, "Init",
                                                "(Landroid/content/Context;)V");
  if (initMethod) {
    Log("GPUPixel",
        "Found Init method, but we're not calling it because we don't have "
        "Context object");
  }

  // Get getResource_path static method
  jmethodID getResourcePathMethod = env->GetStaticMethodID(
      gpuPixelClass, "getResource_path", "()Ljava/lang/String;");
  if (!getResourcePathMethod) {
    if (env->ExceptionCheck()) {
      env->ExceptionClear();
    }
    Log("GPUPixel", "Failed to find getResource_path method");
    env->DeleteLocalRef(gpuPixelClass);
    return getDefaultResourcePath(name);
  }

  // Call static method to get resource path
  jstring jPath = (jstring)env->CallStaticObjectMethod(gpuPixelClass,
                                                       getResourcePathMethod);
  if (env->ExceptionCheck()) {
    env->ExceptionClear();
    Log("GPUPixel", "Exception occurred when calling getResource_path");
    env->DeleteLocalRef(gpuPixelClass);
    return getDefaultResourcePath(name);
  }

  if (jPath != NULL) {
    // Convert Java string to C++ string
    const char* pathChars = env->GetStringUTFChars(jPath, NULL);
    if (pathChars) {
      result = pathChars;
      env->ReleaseStringUTFChars(jPath, pathChars);

      Log("GPUPixel", "Got resource path: %s", result.c_str());

      // If given name is not empty, append to path
      if (!name.empty() && !result.empty()) {
        if (result[result.length() - 1] != '/') {
          result += "/";
        }
        result += name;
      }
    }
    env->DeleteLocalRef(jPath);
  } else {
    Log("GPUPixel", "getResource_path returned null");
    result = getDefaultResourcePath(name);
  }

  env->DeleteLocalRef(gpuPixelClass);
  return result;
}

// Helper function to provide default resource path
std::string Util::getDefaultResourcePath(std::string name) {
  std::string defaultPath =
      "/sdcard/Android/data/com.pixpark.gpupixeldemo/files/resource";
  Log("GPUPixel", "Using default resource path: %s", defaultPath.c_str());

  if (!name.empty()) {
    if (defaultPath[defaultPath.length() - 1] != '/') {
      defaultPath += "/";
    }
    defaultPath += name;
  }

  return defaultPath;
}
#endif

std::string Util::str_format(const char* fmt, ...) {
  std::string strResult = "";
  if (NULL != fmt) {
    va_list marker;
    va_start(marker, fmt);
    char* buf = 0;
    int result = vasprintf(&buf, fmt, marker);
    if (!buf) {
      va_end(marker);
      return strResult;
    }

    if (result < 0) {
      free(buf);
      va_end(marker);
      return strResult;
    }

    result = (int)strlen(buf);
    strResult.append(buf, result);
    free(buf);
    va_end(marker);
  }
  return strResult;
}

int64_t Util::nowTimeMs() {
  auto time_now = std::chrono::system_clock::now();
  auto duration_in_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
      time_now.time_since_epoch());
  int64_t ts = duration_in_ms.count();
  return ts;
}

void Util::Log(const std::string& tag, std::string format, ...) {
  char buffer[10240];
  va_list args;
  va_start(args, format);
#if defined(GPUPIXEL_WIN)
  // Use vsnprintf_s on Windows
  vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, format.c_str(), args);
#else
  // Use vsnprintf on other platforms
  vsnprintf(buffer, sizeof(buffer), format.c_str(), args);
#endif
  va_end(args);
#if defined(GPUPIXEL_ANDROID)
  __android_log_print(ANDROID_LOG_INFO, tag.c_str(), "%s", buffer);
#elif defined(GPUPIXEL_IOS) || defined(GPUPIXEL_MAC)
  NSLog(@"%s", buffer);
#elif defined(GPUPIXEL_LINUX)
  printf("%s\n", buffer);
#endif
}

}  // namespace gpupixel
