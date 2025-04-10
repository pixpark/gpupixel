/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include <memory>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
// define something for Windows (32-bit and 64-bit, this part is common)
#ifdef _WIN64
// define something for Windows (64-bit only)
#define GPUPIXEL_WIN
#else
// define something for Windows (32-bit only)
#define GPUPIXEL_WIN
#endif
#elif __APPLE__
#include <TargetConditionals.h>
#if TARGET_IPHONE_SIMULATOR
// iOS, tvOS, or watchOS Simulator
#define GPUPIXEL_IOS
#elif TARGET_OS_MACCATALYST
// Mac's Catalyst (ports iOS API into Mac, like UIKit).
#define GPUPIXEL_IOS
#elif TARGET_OS_IPHONE
// iOS, tvOS, or watchOS device
#define GPUPIXEL_IOS
#elif TARGET_OS_MAC
// Other kinds of Apple platforms
#define GPUPIXEL_MAC
#else
#error "Unknown Apple platform"
#endif
#elif __ANDROID__
// Below __linux__ check should be enough to handle Android,
// but something may be unique to Android.
#define GPUPIXEL_ANDROID
#elif __linux__
// linux
#define GPUPIXEL_LINUX
#elif __unix__  // all unices not caught above
// Unix
#define GPUPIXEL_LINUX
#elif defined(_POSIX_VERSION)
// POSIX
#error "Unknown compiler"
#define GPUPIXEL_POSIX
#else
#error "Unknown compiler"
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifdef _WIN32
#  ifdef BUILDING_GPUPIXEL_DLL
#    define GPUPIXEL_API __declspec(dllexport)
#  else 
#    define GPUPIXEL_API __declspec(dllimport)
#  endif
#else
#  define GPUPIXEL_API __attribute__((visibility("default")))
#endif 

// Pi
#define PI 3.14159265358979323846264338327950288

namespace gpupixel {

GPUPIXEL_API typedef enum {
  GPUPIXEL_FRAME_TYPE_YUVI420,
  GPUPIXEL_FRAME_TYPE_RGBA,
  GPUPIXEL_FRAME_TYPE_BGRA,
} GPUPIXEL_FRAME_TYPE;

GPUPIXEL_API typedef enum {
  GPUPIXEL_MODE_FMT_VIDEO,
  GPUPIXEL_MODE_FMT_PICTURE,
} GPUPIXEL_MODE_FMT;

}  // namespace gpupixel
