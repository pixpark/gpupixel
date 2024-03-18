/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "gpupixel_macros.h"

#include "math_toolbox.h"
#include <vector>
#include <string>

NS_GPUPIXEL_BEGIN

class GLProgram {
 public:
  GLProgram();
  ~GLProgram();

  static GLProgram* createByShaderString(
      const std::string& vertexShaderSource,
      const std::string& fragmentShaderSource);
  void use();
  GLuint getID() const { return _program; }

  GLuint getAttribLocation(const std::string& attribute);
  GLuint getUniformLocation(const std::string& uniformName);

  void setUniformValue(const std::string& uniformName, int value);
  void setUniformValue(const std::string& uniformName, float value);
  void setUniformValue(const std::string& uniformName, Vector2 value);
  void setUniformValue(const std::string& uniformName, Matrix3 value);
  void setUniformValue(const std::string& uniformName, Matrix4 value);

  void setUniformValue(const std::string& uniformName,
                       const void* array,
                       int length);

  void setUniformValue(int uniformLocation, int value);
  void setUniformValue(int uniformLocation, float value);
  void setUniformValue(int uniformLocation, Vector2 value);
  void setUniformValue(int uniformLocation, Matrix3 value);
  void setUniformValue(int uniformLocation, Matrix4 value);
  void setUniformValue(int uniformLocation, const void* array, int length);

 private:
  static std::vector<GLProgram*> _programs;
  GLuint _program;
  bool _initWithShaderString(const std::string& vertexShaderSource,
                             const std::string& fragmentShaderSource);
};

NS_GPUPIXEL_END
