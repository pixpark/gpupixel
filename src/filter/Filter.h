/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "gl_program.h"
#include "gpupixel_macros.h"
#include "source.h"
#include "target.h"
#include "util.h"
#include "string"

NS_GPUPIXEL_BEGIN

const std::string kDefaultVertexShader = SHADER_STRING(
    attribute vec4 position; attribute vec4 inputTextureCoordinate;

    varying vec2 textureCoordinate;

    void main() {
      gl_Position = position;
      textureCoordinate = inputTextureCoordinate.xy;
    });

#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_ANDROID)
const std::string kDefaultFragmentShader = SHADER_STRING(
    varying highp vec2 textureCoordinate; uniform sampler2D inputImageTexture;

    void main() {
      gl_FragColor = texture2D(inputImageTexture, textureCoordinate);
    });
#elif defined(GPUPIXEL_MAC) || defined(GPUPIXEL_WIN) || defined(GPUPIXEL_LINUX)
const std::string kDefaultFragmentShader = SHADER_STRING(
    varying vec2 textureCoordinate; uniform sampler2D inputImageTexture;

    void main() {
      gl_FragColor = texture2D(inputImageTexture, textureCoordinate);
    });
#endif

class Filter : public Source, public Target {
 public:
  virtual ~Filter();

  static std::shared_ptr<Filter> create(const std::string& filterClassName);

  static std::shared_ptr<Filter> createWithShaderString(
      const std::string& vertexShaderSource,
      const std::string& fragmentShaderSource);

  static std::shared_ptr<Filter> createWithFragmentShaderString(
      const std::string& fragmentShaderSource);

  bool initWithShaderString(const std::string& vertexShaderSource,
                            const std::string& fragmentShaderSource,
                            int inputNumber = 1);

  virtual bool initWithFragmentShaderString(
      const std::string& fragmentShaderSource,
      int inputNumber = 1);

  void setFilterClassName(const std::string filterClassName) {
    _filterClassName = filterClassName;
  }

  std::string getFilterClassName() const { return _filterClassName; };

  virtual void update(int64_t frameTime) override;

  virtual bool proceed(bool bUpdateTargets = true,
                       int64_t frametime = 0) override;

  GLProgram* getProgram() const { return _filterProgram; };

  // property setters & getters
  bool registerProperty(const std::string& name,
                        int defaultValue,
                        const std::string& comment = "",
                        std::function<void(int&)> setCallback = 0);

  bool registerProperty(const std::string& name,
                        float defaultValue,
                        const std::string& comment = "",
                        std::function<void(float&)> setCallback = 0);

  bool registerProperty(const std::string& name,
                        const std::string& defaultValue,
                        const std::string& comment = "",
                        std::function<void(std::string&)> setCallback = 0);

  bool setProperty(const std::string& name, int value);

  bool setProperty(const std::string& name, float value);

  bool setProperty(const std::string& name, std::string value);

  bool getProperty(const std::string& name, int& retValue);

  bool getProperty(const std::string& name, float& retValue);

  bool getProperty(const std::string& name, std::string& retValue);

  bool hasProperty(const std::string& name);

  bool hasProperty(const std::string& name, const std::string type);

  bool getPropertyComment(const std::string& name, std::string& retComment);

  bool getPropertyType(const std::string& name, std::string& retType);

 protected:
  GLProgram* _filterProgram;
  GLuint _filterPositionAttribute;
  std::string _filterClassName;
  struct {
    float r;
    float g;
    float b;
    float a;
  } _backgroundColor;

  Filter();

  std::string _getVertexShaderString(int inputNumber) const;

  const GLfloat* _getTexureCoordinate(const RotationMode& rotationMode) const;

  // properties
  struct Property {
    std::string type;
    std::string comment;
  };

  Property* _getProperty(const std::string& name);

  struct IntProperty : Property {
    int value;
    std::function<void(int&)> setCallback;
  };
  std::map<std::string, IntProperty> _intProperties;

  struct FloatProperty : Property {
    float value;
    std::function<void(float&)> setCallback;
  };
  std::map<std::string, FloatProperty> _floatProperties;

  struct StringProperty : Property {
    std::string value;
    std::function<void(std::string&)> setCallback;
  };
  std::map<std::string, StringProperty> _stringProperties;

 private:
  static std::map<std::string, std::function<std::shared_ptr<Filter>()>> _filterFactories;
};

#define REGISTER_FILTER_CLASS(className)

NS_GPUPIXEL_END
