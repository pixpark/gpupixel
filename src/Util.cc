/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "Util.h"
#include "GPUPixelContext.h"

#if defined(GPUPIXEL_ANDROID)
#include <android/log.h>
#elif defined(GPUPIXEL_IOS)
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#elif defined(GPUPIXEL_WIN)
#include <malloc.h>
#include <stdio.h>
#endif
 
#if defined(GPUPIXEL_IOS)
@interface ObjcHelper : NSObject
+ (NSString*)getResourcePath:(NSString*)name;
+ (NSString*)getBundleResourceWithName:(NSString*)bundleName
                              fileName:(NSString*)fileName
                                  type:(NSString*)fileType;
@end

@implementation ObjcHelper

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


NS_GPUPIXEL_BEGIN

#if defined(GPUPIXEL_IOS)
std::string Util::getResourcePath(std::string name) {
  NSString* oc_path = [ObjcHelper
      getResourcePath:[[NSString alloc] initWithUTF8String:name.c_str()]];
  std::string path = [oc_path UTF8String];
  return path;
}

std::string Util::getResourcePath(std::string bundle_name,
                                  std::string file_name,
                                  std::string type) {
  NSString* oc_path = [ObjcHelper
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

#ifdef _WIN32
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

void Util::Log(const std::string& tag, const std::string& format, ...) {
  char buffer[10240];
  va_list args;
  va_start(args, format);
#if defined(GPUPIXEL_WIN)
  // vsprintf(buffer, format.c_str(), args);
#else
  vsprintf(buffer, format.c_str(), args);
#endif
  va_end(args);
#if defined(GPUPIXEL_ANDROID)
  __android_log_print(ANDROID_LOG_INFO, tag.c_str(), "%s", buffer);
#elif defined(GPUPIXEL_IOS)
  NSLog(@"%s", buffer);
#endif
}

NS_GPUPIXEL_END