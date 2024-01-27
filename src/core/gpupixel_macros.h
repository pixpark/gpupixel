/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include <memory>

// version
#define GPUPIXEL_SDK_VERSION "v1.0.6"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
   //define something for Windows (32-bit and 64-bit, this part is common)
   #ifdef _WIN64
      //define something for Windows (64-bit only)
      #define GPUPIXEL_WIN
   #else
      //define something for Windows (32-bit only)
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
    #   error "Unknown Apple platform"
    #endif
#elif __ANDROID__
    // Below __linux__ check should be enough to handle Android,
    // but something may be unique to Android.
    #define GPUPIXEL_ANDROID
#elif __linux__
    // linux
    #define GPUPIXEL_LINUX
#elif __unix__ // all unices not caught above
#   error "Unknown compiler"
    #define GPUPIXEL_UNIX
    // Unix
#elif defined(_POSIX_VERSION)
    // POSIX
#   error "Unknown compiler"
    #define GPUPIXEL_POSIX
#else
#   error "Unknown compiler"
#endif

//
#if defined(GPUPIXEL_WIN)
// #include <corecrt_math_defines.h>
#include <algorithm>
#endif

#define NS_GPUPIXEL_BEGIN namespace gpupixel {
#define NS_GPUPIXEL_END }
#define USING_NS_GPUPIXEL using namespace gpupixel;


// Pi
#define PI 3.14159265358979323846264338327950288

//------------- ENABLE_GL_CHECK Begin ------------ //
#define ENABLE_GL_CHECK true
#if ENABLE_GL_CHECK
  #define CHECK_GL(glFunc)                                                     \
  glFunc;                                                                      \
  {                                                                            \
    int e = glGetError();                                                      \
    if (e != 0) {                                                              \
      std::string errorString = "";                                            \
      switch (e) {                                                             \
        case GL_INVALID_ENUM:                                                  \
          errorString = "GL_INVALID_ENUM";                                     \
          break;                                                               \
        case GL_INVALID_VALUE:                                                 \
          errorString = "GL_INVALID_VALUE";                                    \
          break;                                                               \
        case GL_INVALID_OPERATION:                                             \
          errorString = "GL_INVALID_OPERATION";                                \
          break;                                                               \
        case GL_OUT_OF_MEMORY:                                                 \
          errorString = "GL_OUT_OF_MEMORY";                                    \
          break;                                                               \
        default:                                                               \
          break;                                                               \
      }                                                                        \
      gpupixel::Util::Log("ERROR", "GL ERROR 0x%04X %s in func:%s(), in file:%s, at line %i", e, \
                          errorString.c_str(), __FUNCTION__, __FILE__, __LINE__);        \
    }                                                                          \
  }
#else
  #define CHECK_GL(glFunc) glFunc;
#endif
 
