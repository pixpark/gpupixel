/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "gpupixel_program.h"
#include "gpupixel_macros.h"
#include "source.h"
#include "sink.h"
#include "util.h"
#include "string"

namespace gpupixel {
const std::string kDefaultVertexShader = R"(
    attribute vec4 position; attribute vec4 inputTextureCoordinate;

    varying vec2 textureCoordinate;

    void main() {
      gl_Position = position;
      textureCoordinate = inputTextureCoordinate.xy;
    })";

#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_ANDROID)
GPUPIXEL_API const std::string kDefaultFragmentShader = R"(
    varying highp vec2 textureCoordinate; uniform sampler2D inputImageTexture;

    void main() {
      gl_FragColor = texture2D(inputImageTexture, textureCoordinate);
    })";
#elif defined(GPUPIXEL_MAC) || defined(GPUPIXEL_WIN) || defined(GPUPIXEL_LINUX)
const std::string kDefaultFragmentShader = R"(
    varying vec2 textureCoordinate; uniform sampler2D inputImageTexture;

    void main() {
      gl_FragColor = texture2D(inputImageTexture, textureCoordinate);
    })";
#endif

class GPUPIXEL_API Filter : public Source, public Sink {
 public:
  virtual ~Filter();

  static std::shared_ptr<Filter> Create(const std::string& filterClassName);

  static std::shared_ptr<Filter> CreateWithShaderString(
      const std::string& vertexShaderSource,
      const std::string& fragmentShaderSource);

  static std::shared_ptr<Filter> CreateWithFragmentShaderString(
      const std::string& fragmentShaderSource);

  bool InitWithShaderString(const std::string& vertexShaderSource,
                            const std::string& fragmentShaderSource,
                            int inputNumber = 1);

  virtual bool InitWithFragmentShaderString(
      const std::string& fragmentShaderSource,
      int inputNumber = 1);

  void SetFilterClassName(const std::string filterClassName) {
    _filterClassName = filterClassName;
  }

  std::string GetFilterClassName() const { return _filterClassName; };

  virtual void Render() override;

  virtual bool DoRender(bool updateSinks = true) override;

  GPUPixelGLProgram* GetGlProgram() const { return _filterProgram; };

  // property setters & getters
  bool RegisterProperty(const std::string& name,
                        int defaultValue,
                        const std::string& comment = "",
                        std::function<void(int&)> setCallback = 0);

  bool RegisterProperty(const std::string& name,
                        float defaultValue,
                        const std::string& comment = "",
                        std::function<void(float&)> setCallback = 0);

bool RegisterProperty(
        const std::string& name,
        std::vector<float> defaultValue,
        const std::string& comment /* = ""*/,
        std::function<void(std::vector<float>)> setCallback /* = 0*/);

  bool RegisterProperty(const std::string& name,
                        const std::string& defaultValue,
                        const std::string& comment = "",
                        std::function<void(std::string&)> setCallback = 0);

  bool SetProperty(const std::string& name, int value);

  bool SetProperty(const std::string& name, float value);

  bool SetProperty(const std::string& name, std::string value);

  bool GetProperty(const std::string& name, int& retValue);

  bool GetProperty(const std::string& name, float& retValue);

  bool SetProperty(const std::string& name, std::vector<float> value);

  bool GetProperty(const std::string& name, std::string& retValue);

  bool HasProperty(const std::string& name);

  bool HasProperty(const std::string& name, const std::string type);

  bool GetPropertyComment(const std::string& name, std::string& retComment);

  bool GetPropertyType(const std::string& name, std::string& retType);

 protected:
  GPUPixelGLProgram* _filterProgram;
  GLuint _filterPositionAttribute;
  std::string _filterClassName;
  struct {
    float r;
    float g;
    float b;
    float a;
  } background_color_;

  Filter();

  std::string _getVertexShaderString(int inputNumber) const;

  const GLfloat* GetTexureCoordinate(const RotationMode& rotationMode) const;

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

  struct VectorProperty : Property {
      std::vector<float> value;
      std::function<void(std::vector<float>&)> setCallback;
  };
  std::map<std::string, VectorProperty> _vectorProperties;


    struct StringProperty : Property {
    std::string value;
    std::function<void(std::string&)> setCallback;
  };
  std::map<std::string, StringProperty> _stringProperties;

 private:
  static std::map<std::string, std::function<std::shared_ptr<Filter>()>> _filterFactories;
};

#define REGISTER_FILTER_CLASS(className)

}
