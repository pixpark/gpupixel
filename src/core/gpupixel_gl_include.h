/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "gpupixel/gpupixel_define.h"
#include "utils/logging.h"
#if defined(GPUPIXEL_IOS)
#import <OpenGLES/ES3/gl.h>
#import <OpenGLES/ES3/glext.h>
#import <UIKit/UIKit.h>
#elif defined(GPUPIXEL_MAC)
#import <AppKit/AppKit.h>
#import <OpenGL/gl.h>
#elif defined(GPUPIXEL_ANDROID)
#include <EGL/egl.h>
#include <GLES/gl.h>
#include <GLES/glext.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include <android/log.h>
#include <jni.h>
#elif defined(GPUPIXEL_WIN) || defined(GPUPIXEL_LINUX)
// clang-format off
#include <glad/glad.h>
#define GLEW_STATIC
#include <GLFW/glfw3.h>
// clang-format on
#elif defined(GPUPIXEL_WASM)
#include <GLES3/gl3.h>
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

// clang-format off
//------------- ENABLE_GL_CHECK Begin ------------ //
#if defined(NDEBUG)
#define GPUPIXEL_OPENGL_DEBUG false
#else
#define GPUPIXEL_OPENGL_DEBUG true
#endif

#if GPUPIXEL_OPENGL_DEBUG
#define GL_CALL(_CALL)                                                          \
  do {                                                                          \
    _CALL;                                                                      \
    GLenum e = glGetError();                                                    \
    if (e != 0) {                                                               \
      std::string errorString;                                                  \
      switch (e) {                                                              \
        case GL_INVALID_ENUM:      errorString = "GL_INVALID_ENUM";      break; \
        case GL_INVALID_VALUE:     errorString = "GL_INVALID_VALUE";     break; \
        case GL_INVALID_OPERATION: errorString = "GL_INVALID_OPERATION"; break; \
        case GL_OUT_OF_MEMORY:    errorString = "GL_OUT_OF_MEMORY";     break;  \
        default:                  errorString = "Unknown GL Error";     break;  \
      }                                                                         \
      LOG_ERROR("[{} {}:{}] GL ERROR: 0x{:04X} MSG: {}", __FILE__,              \
                __FUNCTION__, __LINE__, e, errorString.c_str());                \
    }                                                                           \
  } while (0)
#else
#define GL_CALL(_CALL) do { _CALL; } while (0)
#endif
// clang-format on
