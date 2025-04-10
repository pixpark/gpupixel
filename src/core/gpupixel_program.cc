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
    CHECK_GL(glDeleteProgram(program_));
    program_ = -1;
  }
  CHECK_GL(program_ = glCreateProgram());

  CHECK_GL(uint32_t vert_shader = glCreateShader(GL_VERTEX_SHADER));
  const char* vertex_shader_source_str = vertex_shader_source.c_str();
  CHECK_GL(glShaderSource(vert_shader, 1, &vertex_shader_source_str, NULL));
  CHECK_GL(glCompileShader(vert_shader));

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
    gpupixel::Util::Log(
        "ERROR",
        "GL ERROR GPUPixelGLProgram::InitWithShaderString vertex shader %s",
        messages);
    return -1;
  }

  CHECK_GL(uint32_t frag_shader = glCreateShader(GL_FRAGMENT_SHADER));
  const char* fragment_shader_source_str = fragment_shader_source.c_str();
  CHECK_GL(glShaderSource(frag_shader, 1, &fragment_shader_source_str, NULL));
  CHECK_GL(glCompileShader(frag_shader));

  glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &compile_success);
  if (compile_success == GL_FALSE) {
    GLchar messages[256];
    glGetShaderInfoLog(frag_shader, sizeof(messages), 0, &messages[0]);
#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_MAC)
    NSString* message_string = [NSString stringWithUTF8String:messages];
    NSLog(@"%@", message_string);
#else

#endif
    gpupixel::Util::Log(
        "ERROR",
        "GL ERROR GPUPixelGLProgram::InitWithShaderString frag shader %s",
        messages);
    return -1;
  }

  CHECK_GL(glAttachShader(program_, vert_shader));
  CHECK_GL(glAttachShader(program_, frag_shader));

  CHECK_GL(glLinkProgram(program_));

  CHECK_GL(glDeleteShader(vert_shader));
  CHECK_GL(glDeleteShader(frag_shader));

  return true;
}

void GPUPixelGLProgram::UseProgram() {
  CHECK_GL(glUseProgram(program_));
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
  CHECK_GL(glUniform1i(uniform_location, value));
}

void GPUPixelGLProgram::SetUniformValue(int uniform_location, float value) {
  GPUPixelContext::GetInstance()->SetActiveGlProgram(this);
  CHECK_GL(glUniform1f(uniform_location, value));
}

void GPUPixelGLProgram::SetUniformValue(int uniform_location, Matrix4 value) {
  GPUPixelContext::GetInstance()->SetActiveGlProgram(this);
  CHECK_GL(glUniformMatrix4fv(uniform_location, 1, GL_FALSE, (float*)&value));
}

void GPUPixelGLProgram::SetUniformValue(int uniform_location, Vector2 value) {
  GPUPixelContext::GetInstance()->SetActiveGlProgram(this);
  CHECK_GL(glUniform2f(uniform_location, value.x, value.y));
}

void GPUPixelGLProgram::SetUniformValue(int uniform_location, Matrix3 value) {
  GPUPixelContext::GetInstance()->SetActiveGlProgram(this);
  CHECK_GL(glUniformMatrix3fv(uniform_location, 1, GL_FALSE, (float*)&value));
}

void GPUPixelGLProgram::SetUniformValue(int uniform_location,
                                        const void* value,
                                        int length) {
  GPUPixelContext::GetInstance()->SetActiveGlProgram(this);
  CHECK_GL(glUniform1fv(uniform_location, length, (float*)value));
}

}  // namespace gpupixel
