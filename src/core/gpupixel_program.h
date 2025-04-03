/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "gpupixel_define.h"

#include "math_toolbox.h"
#include <vector>
#include <string>

namespace gpupixel {
class GPUPIXEL_API GPUPixelGLProgram {
 public:
  GPUPixelGLProgram();
  ~GPUPixelGLProgram();

  static GPUPixelGLProgram* createByShaderString(
      const std::string& vertexShaderSource,
      const std::string& fragmentShaderSource);
  void UseProgram();
  GLuint GetProgram() const { return _program; }

  GLuint GetAttribLocation(const std::string& attribute);
  GLuint GetUniformLocation(const std::string& uniformName);

  void SetUniformValue(const std::string& uniformName, int value);
  void SetUniformValue(const std::string& uniformName, float value);
  void SetUniformValue(const std::string& uniformName, Vector2 value);
  void SetUniformValue(const std::string& uniformName, Matrix3 value);
  void SetUniformValue(const std::string& uniformName, Matrix4 value);

  void SetUniformValue(const std::string& uniformName,
                       const void* array,
                       int length);

  void SetUniformValue(int uniformLocation, int value);
  void SetUniformValue(int uniformLocation, float value);
  void SetUniformValue(int uniformLocation, Vector2 value);
  void SetUniformValue(int uniformLocation, Matrix3 value);
  void SetUniformValue(int uniformLocation, Matrix4 value);
  void SetUniformValue(int uniformLocation, const void* array, int length);

 private:
  static std::vector<GPUPixelGLProgram*> _programs;
  GLuint _program;
  bool _initWithShaderString(const std::string& vertexShaderSource,
                             const std::string& fragmentShaderSource);
};

}
