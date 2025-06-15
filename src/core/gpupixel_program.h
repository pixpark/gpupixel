/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <vector>
#include "core/gpupixel_gl_include.h"

namespace gpupixel {
class GPUPIXEL_API GPUPixelGLProgram {
 public:
  GPUPixelGLProgram();
  ~GPUPixelGLProgram();

  static GPUPixelGLProgram* CreateWithShaderString(
      const std::string& vertex_shader_source,
      const std::string& fragment_shader_source);
  void UseProgram();
  uint32_t GetProgram() const { return program_; }

  uint32_t GetAttribLocation(const std::string& attribute);
  uint32_t GetUniformLocation(const std::string& uniform_name);

  void SetUniformValue(const std::string& uniform_name, int value);
  void SetUniformValue(const std::string& uniform_name, float value);
  void SetUniformValue(const std::string& uniform_name, glm::vec2 value);
  void SetUniformValue(const std::string& uniform_name, glm::mat3 value);
  void SetUniformValue(const std::string& uniform_name, glm::mat4 value);

  void SetUniformValue(const std::string& uniform_name,
                       const void* array,
                       int length);

  void SetUniformValue(int uniform_location, int value);
  void SetUniformValue(int uniform_location, float value);
  void SetUniformValue(int uniform_location, glm::vec2 value);
  void SetUniformValue(int uniform_location, glm::mat3 value);
  void SetUniformValue(int uniform_location, glm::mat4 value);
  void SetUniformValue(int uniform_location, const void* array, int length);

 private:
  static std::vector<GPUPixelGLProgram*> programs_;
  uint32_t program_;
  bool InitWithShaderString(const std::string& vertex_shader_source,
                            const std::string& fragment_shader_source);
};

}  // namespace gpupixel
