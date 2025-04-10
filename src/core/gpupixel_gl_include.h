/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "gpupixel/gpupixel_define.h"

#if defined(GPUPIXEL_IOS)
#import <OpenGLES/ES3/gl.h>
#import <OpenGLES/ES3/glext.h>
#elif defined(GPUPIXEL_MAC)
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
#include <glad/glad.h>
#define GLEW_STATIC
#include <GLFW/glfw3.h>
#endif

//------------- ENABLE_GL_CHECK Begin ------------ //
#define ENABLE_GL_CHECK true
#if ENABLE_GL_CHECK
#define CHECK_GL(glFunc)                                                      \
  glFunc;                                                                     \
  {                                                                           \
    int e = glGetError();                                                     \
    if (e != 0) {                                                             \
      std::string errorString = "";                                           \
      switch (e) {                                                            \
        case GL_INVALID_ENUM:                                                 \
          errorString = "GL_INVALID_ENUM";                                    \
          break;                                                              \
        case GL_INVALID_VALUE:                                                \
          errorString = "GL_INVALID_VALUE";                                   \
          break;                                                              \
        case GL_INVALID_OPERATION:                                            \
          errorString = "GL_INVALID_OPERATION";                               \
          break;                                                              \
        case GL_OUT_OF_MEMORY:                                                \
          errorString = "GL_OUT_OF_MEMORY";                                   \
          break;                                                              \
        default:                                                              \
          break;                                                              \
      }                                                                       \
      gpupixel::Util::Log(                                                    \
          "ERROR", "GL ERROR 0x%04X %s in func:%s(), in file:%s, at line %i", \
          e, errorString.c_str(), __FUNCTION__, __FILE__, __LINE__);          \
    }                                                                         \
  }
#else
#define CHECK_GL(glFunc) glFunc;
#endif
