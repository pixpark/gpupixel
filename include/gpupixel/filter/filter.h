/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "gpupixel/gpupixel_define.h"
#include "gpupixel/sink/sink.h"
#include "gpupixel/source/source.h"
#include "gpupixel/utils/math_toolbox.h"

#include <string>
#include <vector>
namespace gpupixel {
class GPUPixelGLProgram;
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

  static std::shared_ptr<Filter> Create(const std::string& filter_class_name);

  static std::shared_ptr<Filter> CreateWithShaderString(
      const std::string& vertex_shader_source,
      const std::string& fragment_shader_source);

  static std::shared_ptr<Filter> CreateWithFragmentShaderString(
      const std::string& fragment_shader_source);

  bool InitWithShaderString(const std::string& vertex_shader_source,
                            const std::string& fragment_shader_source,
                            int input_number = 1);

  virtual bool InitWithFragmentShaderString(
      const std::string& fragment_shader_source,
      int input_number = 1);

  void SetFilterClassName(const std::string filter_class_name) {
    filter_class_name_ = filter_class_name;
  }

  std::string GetFilterClassName() const { return filter_class_name_; };

  virtual void Render() override;

  virtual bool DoRender(bool update_sinks = true) override;

  GPUPixelGLProgram* GetGlProgram() const { return filter_program_; };

  // property setters & getters
  bool RegisterProperty(const std::string& name,
                        int default_value,
                        const std::string& comment = "",
                        std::function<void(int&)> on_property_set_func = 0);

  bool RegisterProperty(const std::string& name,
                        float default_value,
                        const std::string& comment = "",
                        std::function<void(float&)> on_property_set_func = 0);

  bool RegisterProperty(
      const std::string& name,
      std::vector<float> default_value,
      const std::string& comment /* = ""*/,
      std::function<void(std::vector<float>&)> on_property_set_func /* = 0*/);

  bool RegisterProperty(
      const std::string& name,
      const std::string& default_value,
      const std::string& comment = "",
      std::function<void(std::string&)> on_property_set_func = 0);

  bool SetProperty(const std::string& name, int value);

  bool SetProperty(const std::string& name, float value);

  bool SetProperty(const std::string& name, std::string value);

  bool GetProperty(const std::string& name, int& ret_value);

  bool GetProperty(const std::string& name, float& ret_value);

  bool SetProperty(const std::string& name, std::vector<float> value);

  bool GetProperty(const std::string& name, std::string& ret_value);

  bool HasProperty(const std::string& name);

  bool HasProperty(const std::string& name, const std::string type);

  bool GetPropertyComment(const std::string& name, std::string& ret_comment);

  bool GetPropertyType(const std::string& name, std::string& ret_type);

 protected:
  GPUPixelGLProgram* filter_program_;
  uint32_t filter_position_attribute_;
  std::string filter_class_name_;
  struct {
    float r;
    float g;
    float b;
    float a;
  } background_color_;

  Filter();

  std::string GetVertexShaderString(int input_number) const;

  const float* GetTextureCoordinate(const RotationMode& rotation_mode) const;

  // properties
  struct Property {
    std::string type;
    std::string comment;
  };

  Property* GetProperty(const std::string& name);

  struct IntProperty : Property {
    int value;
    std::function<void(int&)> on_property_set_func;
  };
  std::map<std::string, IntProperty> int_properties_;

  struct FloatProperty : Property {
    float value;
    std::function<void(float&)> on_property_set_func;
  };
  std::map<std::string, FloatProperty> float_properties_;

  struct VectorProperty : Property {
    std::vector<float> value;
    std::function<void(std::vector<float>&)> on_property_set_func;
  };
  std::map<std::string, VectorProperty> vector_properties_;

  struct StringProperty : Property {
    std::string value;
    std::function<void(std::string&)> on_property_set_func;
  };
  std::map<std::string, StringProperty> string_properties_;

 private:
  static std::map<std::string, std::function<std::shared_ptr<Filter>()>>
      filter_factories_;
};

}  // namespace gpupixel
