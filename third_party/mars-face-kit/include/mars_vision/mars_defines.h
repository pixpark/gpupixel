//
//  mars-face-kit
//
//  Created by PixPark on 2024/11/18.
//
#pragma once

#include <memory>
#include <string>
#include <vector>
// version
#define MARS_VISION_SDK_VERSION "v0.0.2"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
// define something for Windows (32-bit and 64-bit, this part is common)
#ifdef _WIN64
// define something for Windows (64-bit only)
#define MARS_VISION_WIN
#else
// define something for Windows (32-bit only)
#define MARS_VISION_WIN
#endif
#elif __APPLE__
#include <TargetConditionals.h>
#if TARGET_IPHONE_SIMULATOR
// iOS, tvOS, or watchOS Simulator
#define MARS_VISION_IOS
#elif TARGET_OS_MACCATALYST
// Mac's Catalyst (ports iOS API into Mac, like UIKit).
#define MARS_VISION_IOS
#elif TARGET_OS_IPHONE
// iOS, tvOS, or watchOS device
#define MARS_VISION_IOS
#elif TARGET_OS_MAC
// Other kinds of Apple platforms
#define MARS_VISION_MAC
#else
#error "Unknown Apple platform"
#endif
#elif __ANDROID__
// Below __linux__ check should be enough to handle Android,
// but something may be unique to Android.
#define MARS_VISION_ANDROID
#elif __linux__
// linux
#define MARS_VISION_LINUX
#elif __emscripten__
#define MARS_VISION_LINUX
#elif __unix__  // all unices not caught above
// Unix
#define MARS_VISION_LINUX
#elif defined(_POSIX_VERSION)
// POSIX
#error "Unknown Platform"
#define MARS_VISION_POSIX
#else
#error "Unknown Platform"
#endif

#ifndef MARS_VISION_API
#ifdef MARS_VISION_WIN
#define MARS_VISION_API __declspec(dllexport)
#else
#define MARS_VISION_API
#endif
#endif

namespace mars_vision {
struct Point2d {
  float x;
  float y;
};

struct Point3d {
  float x;
  float y;
  float z;
};

struct Rect {
  float x;
  float y;
  float width;
  float height;
};

enum MarsImageFormat {
  RGBA = 0,
  RGB = 1,
  BGR = 2,
  GRAY = 3,
  BGRA = 4,
  YUV_I420 = 8
};

enum class RotateType {
  CLOCKWISE_0 = 0,
  CLOCKWISE_90 = 1,
  CLOCKWISE_180 = 2,
  CLOCKWISE_270 = 3,
};

struct MarsImage {
  void CopyImage(const MarsImage& image) {
    int data_len = 0;
    switch (image.format) {
      case MarsImageFormat::RGBA:
      case MarsImageFormat::BGRA:
        data_len = image.stride * image.height;
        break;
      case MarsImageFormat::RGB:
      case MarsImageFormat::BGR:
        data_len = image.stride * image.height;
        break;
      case MarsImageFormat::GRAY:
        data_len = image.stride * image.height;
        break;
      case MarsImageFormat::YUV_I420:
        data_len = image.stride * image.height * 3 / 2;
        break;
    }
    data = new uint8_t[data_len];
    memcpy(data, image.data, data_len);
    format = image.format;
    width = image.width;
    height = image.height;
    stride = image.stride;
    timestamp = image.timestamp;
    rotate_type = image.rotate_type;
  };

  uint8_t* data;
  MarsImageFormat format;
  int width;
  int height;
  int stride;
  double timestamp;
  RotateType rotate_type;
};

}  // namespace mars_vision
