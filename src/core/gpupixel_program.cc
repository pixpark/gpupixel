/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "core/gpupixel_program.h"
#include <algorithm>
#include "core/gpupixel_context.h"
#include "utils/util.h"

namespace gpupixel {

std::vector<GPUPixelGLProgram*> GPUPixelGLProgram::programs_;

GPUPixelGLProgram::GPUPixelGLProgram() : program_(-1) {
  programs_.push_back(this);
}

GPUPixelGLProgram::~GPUPixelGLProgram() {
  GPUPixelContext::GetInstance()->SyncRunWithContext([=] {
    std::vector<GPUPixelGLProgram*>::iterator itr =
        std::find(programs_.begin(), programs_.end(), this);
    if (itr != programs_.end()) {
      programs_.erase(itr);
    }

    bool should_delete_program = (program_ != -1);

    for (auto const& program : programs_) {
      if (should_delete_program) {
        if (program_ == program->GetProgram()) {
          should_delete_program = false;
          break;
        }
      }
    }

    if (should_delete_program) {
      glDeleteProgram(program_);
      program_ = -1;
    }
  });
}

GPUPixelGLProgram* GPUPixelGLProgram::CreateWithShaderString(
    const std::string& vertex_shader_source,
    const std::string& fragment_shader_source) {
  GPUPixelGLProgram* ret = new (std::nothrow) GPUPixelGLProgram();
  if (ret) {
    if (!ret->InitWithShaderString(vertex_shader_source,
                                   fragment_shader_source)) {
      delete ret;
      ret = nullptr;
    }
  }
  return ret;
}

bool GPUPixelGLProgram::InitWithShaderString(
    const std::string& vertex_shader_source,
    const std::string& fragment_shader_source) {
  if (program_ != -1) {
    GL_CALL(glDeleteProgram(program_));
    program_ = -1;
  }
  GL_CALL(program_ = glCreateProgram());

  uint32_t vert_shader;
  GL_CALL(vert_shader = glCreateShader(GL_VERTEX_SHADER));
  const char* vertex_shader_source_str = vertex_shader_source.c_str();
  GL_CALL(glShaderSource(vert_shader, 1, &vertex_shader_source_str, NULL));
  GL_CALL(glCompileShader(vert_shader));

  //
  GLint compile_success;
  glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &compile_success);
  if (compile_success == GL_FALSE) {
    GLchar messages[256];
    glGetShaderInfoLog(vert_shader, sizeof(messages), 0, &messages[0]);
#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_MAC)
    NSString* message_string = [NSString stringWithUTF8String:messages];
    NSLog(@"%@", message_string);
#else

#endif
    LOG_ERROR(
        "GL ERROR GPUPixelGLProgram::InitWithShaderString vertex shader {}",
        messages);
    return -1;
  }

  uint32_t frag_shader;
  GL_CALL(frag_shader = glCreateShader(GL_FRAGMENT_SHADER));
  const char* fragment_shader_source_str = fragment_shader_source.c_str();
  GL_CALL(glShaderSource(frag_shader, 1, &fragment_shader_source_str, NULL));
  GL_CALL(glCompileShader(frag_shader));

  glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &compile_success);
  if (compile_success == GL_FALSE) {
    GLchar messages[256];
    glGetShaderInfoLog(frag_shader, sizeof(messages), 0, &messages[0]);
#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_MAC)
    NSString* message_string = [NSString stringWithUTF8String:messages];
    NSLog(@"%@", message_string);
#else

#endif
    LOG_ERROR("GL ERROR GPUPixelGLProgram::InitWithShaderString frag shader {}",
              messages);
    return -1;
  }

  GL_CALL(glAttachShader(program_, vert_shader));
  GL_CALL(glAttachShader(program_, frag_shader));

  GL_CALL(glLinkProgram(program_));

  GL_CALL(glDeleteShader(vert_shader));
  GL_CALL(glDeleteShader(frag_shader));

  return true;
}

void GPUPixelGLProgram::UseProgram() {
  GL_CALL(glUseProgram(program_));
}

uint32_t GPUPixelGLProgram::GetAttribLocation(const std::string& attribute) {
  return glGetAttribLocation(program_, attribute.c_str());
}

uint32_t GPUPixelGLProgram::GetUniformLocation(
    const std::string& uniform_name) {
  return glGetUniformLocation(program_, uniform_name.c_str());
}

void GPUPixelGLProgram::SetUniformValue(const std::string& uniform_name,
                                        int value) {
  GPUPixelContext::GetInstance()->SetActiveGlProgram(this);
  SetUniformValue(GetUniformLocation(uniform_name), value);
}

void GPUPixelGLProgram::SetUniformValue(const std::string& uniform_name,
                                        float value) {
  GPUPixelContext::GetInstance()->SetActiveGlProgram(this);
  SetUniformValue(GetUniformLocation(uniform_name), value);
}

void GPUPixelGLProgram::SetUniformValue(const std::string& uniform_name,
                                        Matrix4 value) {
  GPUPixelContext::GetInstance()->SetActiveGlProgram(this);
  SetUniformValue(GetUniformLocation(uniform_name), value);
}

void GPUPixelGLProgram::SetUniformValue(const std::string& uniform_name,
                                        Vector2 value) {
  GPUPixelContext::GetInstance()->SetActiveGlProgram(this);
  SetUniformValue(GetUniformLocation(uniform_name), value);
}

void GPUPixelGLProgram::SetUniformValue(const std::string& uniform_name,
                                        float f1, float f2, float f3, float f4) {
  GPUPixelContext::GetInstance()->SetActiveGlProgram(this);
  SetUniformValue(GetUniformLocation(uniform_name), f1, f2, f3, f4);
}

void GPUPixelGLProgram::SetUniformValue(const std::string& uniform_name,
                                        Matrix3 value) {
  GPUPixelContext::GetInstance()->SetActiveGlProgram(this);
  SetUniformValue(GetUniformLocation(uniform_name), value);
}

void GPUPixelGLProgram::SetUniformValue(const std::string& uniform_name,
                                        const void* value,
                                        int length) {
  GPUPixelContext::GetInstance()->SetActiveGlProgram(this);
  SetUniformValue(GetUniformLocation(uniform_name), value, length);
}

void GPUPixelGLProgram::SetUniformValue(int uniform_location, int value) {
  GPUPixelContext::GetInstance()->SetActiveGlProgram(this);
  GL_CALL(glUniform1i(uniform_location, value));
}

void GPUPixelGLProgram::SetUniformValue(int uniform_location, float value) {
  GPUPixelContext::GetInstance()->SetActiveGlProgram(this);
  GL_CALL(glUniform1f(uniform_location, value));
}

void GPUPixelGLProgram::SetUniformValue(int uniform_location, Matrix4 value) {
  GPUPixelContext::GetInstance()->SetActiveGlProgram(this);
  GL_CALL(glUniformMatrix4fv(uniform_location, 1, GL_FALSE, (float*)&value));
}

void GPUPixelGLProgram::SetUniformValue(int uniform_location, Vector2 value) {
  GPUPixelContext::GetInstance()->SetActiveGlProgram(this);
  GL_CALL(glUniform2f(uniform_location, value.x, value.y));
}

void GPUPixelGLProgram::SetUniformValue(int uniform_location, float f1, float f2, float f3, float f4) {
  GPUPixelContext::GetInstance()->SetActiveGlProgram(this);
  GL_CALL(glUniform4f(uniform_location, f1, f2, f3, f4));
}

void GPUPixelGLProgram::SetUniformValue(int uniform_location, Matrix3 value) {
  GPUPixelContext::GetInstance()->SetActiveGlProgram(this);
  GL_CALL(glUniformMatrix3fv(uniform_location, 1, GL_FALSE, (float*)&value));
}

void GPUPixelGLProgram::SetUniformValue(int uniform_location,
                                        const void* value,
                                        int length) {
  GPUPixelContext::GetInstance()->SetActiveGlProgram(this);
  GL_CALL(glUniform1fv(uniform_location, length, (float*)value));
}

}  // namespace gpupixel
