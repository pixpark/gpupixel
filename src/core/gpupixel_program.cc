/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "gpupixel_program.h"
#include <algorithm>
#include "gpupixel_context.h"
#include "util.h"

namespace gpupixel {

std::vector<GPUPixelGLProgram*> GPUPixelGLProgram::_programs;

GPUPixelGLProgram::GPUPixelGLProgram() : _program(-1) {
  _programs.push_back(this);
}

GPUPixelGLProgram::~GPUPixelGLProgram() {
  GPUPixelContext::GetInstance()->RunSync([=] {
    std::vector<GPUPixelGLProgram*>::iterator itr =
        std::find(_programs.begin(), _programs.end(), this);
    if (itr != _programs.end()) {
      _programs.erase(itr);
    }

    bool bDeleteProgram = (_program != -1);

    for (auto const& program : _programs) {
      if (bDeleteProgram) {
        if (_program == program->GetProgram()) {
          bDeleteProgram = false;
          break;
        }
      }
    }

    if (bDeleteProgram) {
      glDeleteProgram(_program);
      _program = -1;
    }
  });
}

GPUPixelGLProgram* GPUPixelGLProgram::createByShaderString(
    const std::string& vertexShaderSource,
    const std::string& fragmentShaderSource) {
  GPUPixelGLProgram* ret = new (std::nothrow) GPUPixelGLProgram();
  if (ret) {
    if (!ret->_initWithShaderString(vertexShaderSource, fragmentShaderSource)) {
      delete ret;
      ret = 0;
    }
  }
  return ret;
}

bool GPUPixelGLProgram::_initWithShaderString(const std::string& vertexShaderSource,
                                      const std::string& fragmentShaderSource) {
  if (_program != -1) {
    CHECK_GL(glDeleteProgram(_program));
    _program = -1;
  }
  CHECK_GL(_program = glCreateProgram());

  CHECK_GL(GLuint vertShader = glCreateShader(GL_VERTEX_SHADER));
  const char* vertexShaderSourceStr = vertexShaderSource.c_str();
  CHECK_GL(glShaderSource(vertShader, 1, &vertexShaderSourceStr, NULL));
  CHECK_GL(glCompileShader(vertShader));

  //
  GLint compileSuccess;
  glGetShaderiv(vertShader, GL_COMPILE_STATUS, &compileSuccess);
  if (compileSuccess == GL_FALSE) {
    GLchar messages[256];
    glGetShaderInfoLog(vertShader, sizeof(messages), 0, &messages[0]);
#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_MAC)
    NSString* messageString = [NSString stringWithUTF8String:messages];
    NSLog(@"%@", messageString);
#else

#endif
    gpupixel::Util::Log(
        "ERROR", "GL ERROR GPUPixelGLProgram::_initWithShaderString vertex shader %s",
        messages);
    return -1;
  }

  CHECK_GL(GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER));
  const char* fragmentShaderSourceStr = fragmentShaderSource.c_str();
  CHECK_GL(glShaderSource(fragShader, 1, &fragmentShaderSourceStr, NULL));
  CHECK_GL(glCompileShader(fragShader));

  glGetShaderiv(fragShader, GL_COMPILE_STATUS, &compileSuccess);
  if (compileSuccess == GL_FALSE) {
    GLchar messages[256];
    glGetShaderInfoLog(fragShader, sizeof(messages), 0, &messages[0]);
#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_MAC)
    NSString* messageString = [NSString stringWithUTF8String:messages];
    NSLog(@"%@", messageString);
#else

#endif
    gpupixel::Util::Log(
        "ERROR", "GL ERROR GPUPixelGLProgram::_initWithShaderString frag shader %s",
        messages);
    return -1;
  }

  CHECK_GL(glAttachShader(_program, vertShader));
  CHECK_GL(glAttachShader(_program, fragShader));

  CHECK_GL(glLinkProgram(_program));

  CHECK_GL(glDeleteShader(vertShader));
  CHECK_GL(glDeleteShader(fragShader));

  return true;
}

void GPUPixelGLProgram::UseProgram() {
  CHECK_GL(glUseProgram(_program));
}

GLuint GPUPixelGLProgram::GetAttribLocation(const std::string& attribute) {
  return glGetAttribLocation(_program, attribute.c_str());
}

GLuint GPUPixelGLProgram::GetUniformLocation(const std::string& uniformName) {
  return glGetUniformLocation(_program, uniformName.c_str());
}

void GPUPixelGLProgram::SetUniformValue(const std::string& uniformName, int value) {
  GPUPixelContext::GetInstance()->SetActiveGlProgram(this);
  SetUniformValue(GetUniformLocation(uniformName), value);
}

void GPUPixelGLProgram::SetUniformValue(const std::string& uniformName, float value) {
  GPUPixelContext::GetInstance()->SetActiveGlProgram(this);
  SetUniformValue(GetUniformLocation(uniformName), value);
}

void GPUPixelGLProgram::SetUniformValue(const std::string& uniformName, Matrix4 value) {
  GPUPixelContext::GetInstance()->SetActiveGlProgram(this);
  SetUniformValue(GetUniformLocation(uniformName), value);
}

void GPUPixelGLProgram::SetUniformValue(const std::string& uniformName, Vector2 value) {
  GPUPixelContext::GetInstance()->SetActiveGlProgram(this);
  SetUniformValue(GetUniformLocation(uniformName), value);
}

void GPUPixelGLProgram::SetUniformValue(const std::string& uniformName, Matrix3 value) {
  GPUPixelContext::GetInstance()->SetActiveGlProgram(this);
  SetUniformValue(GetUniformLocation(uniformName), value);
}

void GPUPixelGLProgram::SetUniformValue(const std::string& uniformName,
                                const void* value,
                                int length) {
  GPUPixelContext::GetInstance()->SetActiveGlProgram(this);
  SetUniformValue(GetUniformLocation(uniformName), value, length);
}

void GPUPixelGLProgram::SetUniformValue(int uniformLocation, int value) {
  GPUPixelContext::GetInstance()->SetActiveGlProgram(this);
  CHECK_GL(glUniform1i(uniformLocation, value));
}

void GPUPixelGLProgram::SetUniformValue(int uniformLocation, float value) {
  GPUPixelContext::GetInstance()->SetActiveGlProgram(this);
  CHECK_GL(glUniform1f(uniformLocation, value));
}

void GPUPixelGLProgram::SetUniformValue(int uniformLocation, Matrix4 value) {
  GPUPixelContext::GetInstance()->SetActiveGlProgram(this);
  CHECK_GL(glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, (GLfloat*)&value));
}

void GPUPixelGLProgram::SetUniformValue(int uniformLocation, Vector2 value) {
  GPUPixelContext::GetInstance()->SetActiveGlProgram(this);
  CHECK_GL(glUniform2f(uniformLocation, value.x, value.y));
}

void GPUPixelGLProgram::SetUniformValue(int uniformLocation, Matrix3 value) {
  GPUPixelContext::GetInstance()->SetActiveGlProgram(this);
  CHECK_GL(glUniformMatrix3fv(uniformLocation, 1, GL_FALSE, (GLfloat*)&value));
}

void GPUPixelGLProgram::SetUniformValue(int uniformLocation,
                                const void* value,
                                int length) {
  GPUPixelContext::GetInstance()->SetActiveGlProgram(this);
  CHECK_GL(glUniform1fv(uniformLocation, length, (GLfloat*)value));
}

}
