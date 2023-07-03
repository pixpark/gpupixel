/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include <memory>

#if defined(_WIN32)
#include <corecrt_math_defines.h>
#include <algorithm>
#endif

#define NS_GPUPIXEL_BEGIN namespace GPUPixel {
#define NS_GPUPIXEL_END }
#define USING_NS_GPUPIXEL using namespace GPUPixel;

#define STRINGIZE(x) #x
#define SHADER_STRING(text) STRINGIZE(text)

#define PI 3.14159265358979323846264338327950288

#define ENABLE_GL_CHECK true

#if ENABLE_GL_CHECK
#ifdef _WIN32
#define CHECK_GL(glFunc)                                                       \
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
      GPUPixel::Util::Log("ERROR", "GL ERROR 0x%04X %s in %s at line %i\n", e, \
                          errorString.c_str(), __FUNCTION__, __LINE__);        \
    }                                                                          \
  }
#else
#define CHECK_GL(glFunc)                                                       \
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
      GPUPixel::Util::Log("ERROR", "GL ERROR 0x%04X %s in %s at line %i\n", e, \
                          errorString.c_str(), __PRETTY_FUNCTION__, __LINE__); \
    }                                                                          \
  }
#endif
#else
#define CHECK_GL(glFunc) glFunc;
#endif

// -------------
// #if defined(__ANDROID__) || defined(ANDROID)
//    #define GPUPIXEL_ANDROID
// #elif defined(__APPLE__)
//    #if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE
//        #define GPUPIXEL_IOS
//    #else
//        #define GPUPIXEL_MAC
//    #endif
// #endif
