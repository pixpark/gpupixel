/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "gl_program.h"
#include <algorithm>
#include "gpupixel_context.h"
#include "util.h"

NS_GPUPIXEL_BEGIN

std::vector<GLProgram*> GLProgram::_programs;

GLProgram::GLProgram() : _program(-1) {
  _programs.push_back(this);
}

GLProgram::~GLProgram() {
  GPUPixelContext::getInstance()->runSync([=] {
    std::vector<GLProgram*>::iterator itr =
        std::find(_programs.begin(), _programs.end(), this);
    if (itr != _programs.end()) {
      _programs.erase(itr);
    }

    bool bDeleteProgram = (_program != -1);

    for (auto const& program : _programs) {
      if (bDeleteProgram) {
        if (_program == program->getID()) {
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

GLProgram* GLProgram::createByShaderString(
    const std::string& vertexShaderSource,
    const std::string& fragmentShaderSource) {
  GLProgram* ret = new (std::nothrow) GLProgram();
  gpupixel::Util::Log("DEBUG", "create GLProgram %d", ret);
  if (ret) {
    if (!ret->_initWithShaderString(vertexShaderSource, fragmentShaderSource)) {
      gpupixel::Util::Log("DEBUG", "failed to create GLProgram %d", ret);
      delete ret;
      ret = 0;
    }
  }
  return ret;
}

bool GLProgram::_initWithShaderString(const std::string& vertexShaderSource,
                                      const std::string& fragmentShaderSource) {
  if (_program != -1) {
    CHECK_GL(glDeleteProgram(_program));
    _program = -1;
  }
  CHECK_GL(_program = glCreateProgram());
  gpupixel::Util::Log("DEBUG", "after create and get program id %d", _program);

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
        "ERROR", "GL ERROR GLProgram::_initWithShaderString vertex shader %s",
        messages);
    return false;
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
        "ERROR", "GL ERROR GLProgram::_initWithShaderString frag shader %s",
        messages);
    return false;
  }

  CHECK_GL(glAttachShader(_program, vertShader));
  CHECK_GL(glAttachShader(_program, fragShader));

  CHECK_GL(glLinkProgram(_program));

  CHECK_GL(glDeleteShader(vertShader));
  CHECK_GL(glDeleteShader(fragShader));

  return true;
}

void GLProgram::use() {
  CHECK_GL(glUseProgram(_program));
}

GLuint GLProgram::getAttribLocation(const std::string& attribute) {
  if (_program < 0) {
      gpupixel::Util::Log("DEBUG", "didn't get program id");
  }
  gpupixel::Util::Log("DEBUG", "get program id %d", _program);
  return glGetAttribLocation(_program, attribute.c_str());
}

GLuint GLProgram::getUniformLocation(const std::string& uniformName) {
  return glGetUniformLocation(_program, uniformName.c_str());
}

void GLProgram::setUniformValue(const std::string& uniformName, int value) {
  GPUPixelContext::getInstance()->setActiveShaderProgram(this);
  setUniformValue(getUniformLocation(uniformName), value);
}

void GLProgram::setUniformValue(const std::string& uniformName, float value) {
  GPUPixelContext::getInstance()->setActiveShaderProgram(this);
  setUniformValue(getUniformLocation(uniformName), value);
}

void GLProgram::setUniformValue(const std::string& uniformName, Matrix4 value) {
  GPUPixelContext::getInstance()->setActiveShaderProgram(this);
  setUniformValue(getUniformLocation(uniformName), value);
}

void GLProgram::setUniformValue(const std::string& uniformName, Vector2 value) {
  GPUPixelContext::getInstance()->setActiveShaderProgram(this);
  setUniformValue(getUniformLocation(uniformName), value);
}

void GLProgram::setUniformValue(const std::string& uniformName, Matrix3 value) {
  GPUPixelContext::getInstance()->setActiveShaderProgram(this);
  setUniformValue(getUniformLocation(uniformName), value);
}

void GLProgram::setUniformValue(const std::string& uniformName,
                                const void* value,
                                int length) {
  GPUPixelContext::getInstance()->setActiveShaderProgram(this);
  setUniformValue(getUniformLocation(uniformName), value, length);
}

void GLProgram::setUniformValue(int uniformLocation, int value) {
  GPUPixelContext::getInstance()->setActiveShaderProgram(this);
  CHECK_GL(glUniform1i(uniformLocation, value));
}

void GLProgram::setUniformValue(int uniformLocation, float value) {
  GPUPixelContext::getInstance()->setActiveShaderProgram(this);
  CHECK_GL(glUniform1f(uniformLocation, value));
}

void GLProgram::setUniformValue(int uniformLocation, Matrix4 value) {
  GPUPixelContext::getInstance()->setActiveShaderProgram(this);
  CHECK_GL(glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, (GLfloat*)&value));
}

void GLProgram::setUniformValue(int uniformLocation, Vector2 value) {
  GPUPixelContext::getInstance()->setActiveShaderProgram(this);
  CHECK_GL(glUniform2f(uniformLocation, value.x, value.y));
}

void GLProgram::setUniformValue(int uniformLocation, Matrix3 value) {
  GPUPixelContext::getInstance()->setActiveShaderProgram(this);
  CHECK_GL(glUniformMatrix3fv(uniformLocation, 1, GL_FALSE, (GLfloat*)&value));
}

void GLProgram::setUniformValue(int uniformLocation,
                                const void* value,
                                int length) {
  GPUPixelContext::getInstance()->setActiveShaderProgram(this);
  CHECK_GL(glUniform1fv(uniformLocation, length, (GLfloat*)value));
}

NS_GPUPIXEL_END
