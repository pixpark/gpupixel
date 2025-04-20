/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "utils/util.h"
#include <cstdarg>
#include "core/gpupixel_context.h"
#if defined(GPUPIXEL_ANDROID)
#include <android/log.h>
#include "android/jni/jni_helpers.h"
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
+ (NSString*)GetResourcePath:(NSString*)name;
@end

@implementation GPXObjcHelper

+ (NSString*)GetResourcePath:(NSString*)name {
  NSString* path = [[[NSBundle bundleForClass:self.class] resourcePath]
      stringByAppendingPathComponent:name];
  return path;
}
@end

#endif

namespace gpupixel {

std::string Util::resource_root_path_ = "";

std::string Util::GetResourcePath(std::string name) {
#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_MAC)
  NSString* oc_path = [GPXObjcHelper
      GetResourcePath:[[NSString alloc] initWithUTF8String:name.c_str()]];
  std::string path = [oc_path UTF8String];
#else
  std::string path =
      resource_root_path_.empty() ? name : (resource_root_path_ + "/" + name);
#endif
  return path;
}

void Util::SetResourceRoot(std::string root) {
  resource_root_path_ = root;
}

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
 
std::string Util::StringFormat(const char* fmt, ...) {
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

int64_t Util::NowTimeMs() {
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
