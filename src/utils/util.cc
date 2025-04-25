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
+ (NSString*)getResourcePath;
#if defined(GPUPIXEL_IOS)
+ (bool)isAppActive;
#endif
@end

@implementation GPXObjcHelper {
}

#if defined(GPUPIXEL_IOS)
static bool s_isAppActive = false;

+ (void)load {
  @autoreleasepool {
    // 注册应用状态通知
    [[NSNotificationCenter defaultCenter]
        addObserver:self
           selector:@selector(applicationStateChanged:)
               name:UIApplicationWillResignActiveNotification
             object:nil];
    [[NSNotificationCenter defaultCenter]
        addObserver:self
           selector:@selector(applicationStateChanged:)
               name:UIApplicationDidBecomeActiveNotification
             object:nil];
  }
}

+ (void)applicationStateChanged:(NSNotification*)notification {
  @synchronized(self) {
    if ([notification.name
            isEqualToString:UIApplicationWillResignActiveNotification]) {
      s_isAppActive = false;
    } else {
      s_isAppActive = true;
    }
  }
}

+ (bool)isAppActive {
  @synchronized(self) {
    return s_isAppActive;
  }
}

#endif
+ (NSString*)getResourcePath {
  return [[NSBundle bundleForClass:self.class] resourcePath];
}

@end

#endif

namespace gpupixel {

fs::path Util::resource_root_path_ = "";

fs::path Util::GetResourcePath() {
#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_MAC)
  fs::path default_path =
      fs::path([[GPXObjcHelper getResourcePath] UTF8String]);
#else
  fs::path default_path = fs::current_path();
#endif
  if (resource_root_path_.empty()) {
    LOG_INFO("resource_root_path_ is empty, using default path: {}",
             default_path.string());
    return default_path;
  }
  return resource_root_path_;
}

void Util::SetResourcePath(const fs::path& path) {
  LOG_INFO("SetResourcePath: {}", path.string());
  resource_root_path_ = path;
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

bool Util::IsAppleAppActive() {
#if defined(GPUPIXEL_IOS)
  return [GPXObjcHelper isAppActive];
#else
  return true;
#endif
}

}  // namespace gpupixel
