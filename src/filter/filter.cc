/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "filter.h"
#include "gpupixel.h"
#include "gpupixel_context.h"

namespace gpupixel {

std::map<std::string, std::function<std::shared_ptr<Filter>()>>
init_filter_factory() {
  std::map<std::string, std::function<std::shared_ptr<Filter>()>> factory;
  // Beauty related filters
  factory["BeautyFaceFilter"] = BeautyFaceFilter::Create;
  factory["FaceReshapeFilter"] = FaceReshapeFilter::Create;
  factory["LipstickFilter"] = LipstickFilter::Create;
  factory["BlusherFilter"] = BlusherFilter::Create;
  factory["FaceMakeupFilter"] = FaceMakeupFilter::Create;
  
  // Basic adjustment filters
  // factory["BrightnessFilter"] = BrightnessFilter::Create;
  factory["ContrastFilter"] = ContrastFilter::Create;
  factory["ExposureFilter"] = ExposureFilter::Create;
  factory["SaturationFilter"] = SaturationFilter::Create;
  factory["RGBFilter"] = RGBFilter::Create;
  factory["HueFilter"] = HueFilter::Create;
  factory["HSBFilter"] = HSBFilter::Create;
  factory["ColorInvertFilter"] = ColorInvertFilter::Create;
  factory["WhiteBalanceFilter"] = WhiteBalanceFilter::Create;
  factory["ColorMatrixFilter"] = ColorMatrixFilter::Create;
  
  // Blur filters
  // factory["GaussianBlurFilter"] = GaussianBlurFilter::Create;
  // factory["GaussianBlurMonoFilter"] = GaussianBlurMonoFilter::Create;
  // factory["BoxBlurFilter"] = BoxBlurFilter::Create;
  factory["IOSBlurFilter"] = IOSBlurFilter::Create;
  factory["BilateralFilter"] = BilateralFilter::Create;
  // factory["BilateralMonoFilter"] = BilateralMonoFilter::Create;
  // factory["SingleComponentGaussianBlurFilter"] = SingleComponentGaussianBlurFilter::Create;
  // factory["SingleComponentGaussianBlurMonoFilter"] = SingleComponentGaussianBlurMonoFilter::Create;
  
  // Edge detection and effect filters
  factory["SobelEdgeDetectionFilter"] = SobelEdgeDetectionFilter::Create;
  factory["CannyEdgeDetectionFilter"] = CannyEdgeDetectionFilter::Create;
  factory["DirectionalNonMaximumSuppressionFilter"] = DirectionalNonMaximumSuppressionFilter::Create;
  factory["WeakPixelInclusionFilter"] = WeakPixelInclusionFilter::Create;
  
  // Effect filters
  factory["ToonFilter"] = ToonFilter::Create;
  factory["SmoothToonFilter"] = SmoothToonFilter::Create;
  factory["PosterizeFilter"] = PosterizeFilter::Create;
  factory["PixellationFilter"] = PixellationFilter::Create;
  factory["SketchFilter"] = SketchFilter::Create;
  factory["GlassSphereFilter"] = GlassSphereFilter::Create;
  factory["SphereRefractionFilter"] = SphereRefractionFilter::Create;
  factory["EmbossFilter"] = EmbossFilter::Create;

  // Other processing filters
  factory["LuminanceRangeFilter"] = LuminanceRangeFilter::Create;
  
  return factory;
}
std::map<std::string, std::function<std::shared_ptr<Filter>()>>
    Filter::filter_factories_ = init_filter_factory();

Filter::Filter() : filter_program_(0), filter_class_name_("") {
  background_color_.r = 0.0;
  background_color_.g = 0.0;
  background_color_.b = 0.0;
  background_color_.a = 1.0;
}

Filter::~Filter() {
  if (filter_program_) {
    delete filter_program_;
    filter_program_ = 0;
  }
}

std::shared_ptr<Filter> Filter::Create(const std::string& filter_class_name) {
  for (auto filter : filter_factories_) {
    if (filter.first == filter_class_name) {
      return filter.second();
    }
  }
  return nullptr;
}

std::shared_ptr<Filter> Filter::CreateWithShaderString(
    const std::string& vertex_shader_source,
    const std::string& fragment_shader_source) {
  auto filter = std::shared_ptr<Filter>(new Filter());
  if (!filter->InitWithShaderString(vertex_shader_source,
                                    fragment_shader_source)) {

  }
  return filter;
}

std::shared_ptr<Filter> Filter::CreateWithFragmentShaderString(
    const std::string& fragment_shader_source) {
  auto filter = std::shared_ptr<Filter>(new Filter());
  if (!filter->InitWithFragmentShaderString(fragment_shader_source)) {
  }
  return filter;
}

bool Filter::InitWithShaderString(const std::string& vertex_shader_source,
                                  const std::string& fragment_shader_source,
                                  int input_number /* = 1*/) {
  input_count_ = input_number;
  filter_program_ = GPUPixelGLProgram::CreateWithShaderString(
      vertex_shader_source, fragment_shader_source);
  filter_position_attribute_ = filter_program_->GetAttribLocation("position");
  GPUPixelContext::GetInstance()->SetActiveGlProgram(filter_program_);
  CHECK_GL(glEnableVertexAttribArray(filter_position_attribute_));
  return true;
}

bool Filter::InitWithFragmentShaderString(
    const std::string& fragment_shader_source,
    int input_number /* = 1*/) {
  return InitWithShaderString(GetVertexShaderString(input_number),
                              fragment_shader_source, input_number);
}

std::string Filter::GetVertexShaderString(int input_number) const {
  if (input_number <= 1) {
    return kDefaultVertexShader;
  }

  std::string shader_str =
      "\
                attribute vec4 position;\n\
                attribute vec4 inputTextureCoordinate;\n\
                varying vec2 textureCoordinate;\n\
                ";
  for (int i = 1; i < input_number; ++i) {
    shader_str += Util::str_format(
        "\
                attribute vec4 inputTextureCoordinate%d;\n\
                varying vec2 textureCoordinate%d;\n\
                                ",
        i, i);
  }
  shader_str +=
      "\
                void main()\n\
                {\n\
                    gl_Position = position;\n\
                    textureCoordinate = inputTextureCoordinate.xy;\n\
        ";
  for (int i = 1; i < input_number; ++i) {
    shader_str += Util::str_format(
        "textureCoordinate%d = inputTextureCoordinate%d.xy;\n", i, i);
  }
  shader_str += "}\n";

  return shader_str;
}

bool Filter::DoRender(bool update_sinks) {
  static const GLfloat image_vertices[] = {
      -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f,
  };

  GPUPixelContext::GetInstance()->SetActiveGlProgram(filter_program_);
  framebuffer_->Activate();
  CHECK_GL(glClearColor(background_color_.r, background_color_.g,
                        background_color_.b, background_color_.a));
  CHECK_GL(glClear(GL_COLOR_BUFFER_BIT));
  for (std::map<int, InputFrameBufferInfo>::const_iterator it =
           input_framebuffers_.begin();
       it != input_framebuffers_.end(); ++it) {
    int tex_idx = it->first;
    std::shared_ptr<GPUPixelFramebuffer> fb = it->second.frame_buffer;
    CHECK_GL(glActiveTexture(GL_TEXTURE0 + tex_idx));
    CHECK_GL(glBindTexture(GL_TEXTURE_2D, fb->GetTexture()));
    filter_program_->SetUniformValue(
        tex_idx == 0 ? "inputImageTexture"
                     : Util::str_format("inputImageTexture%d", tex_idx),
        tex_idx);
    // texcoord attribute
    GLuint filter_tex_coord_attribute = filter_program_->GetAttribLocation(
        tex_idx == 0 ? "inputTextureCoordinate"
                     : Util::str_format("inputTextureCoordinate%d", tex_idx));
    CHECK_GL(glEnableVertexAttribArray(filter_tex_coord_attribute));
    CHECK_GL(
        glVertexAttribPointer(filter_tex_coord_attribute, 2, GL_FLOAT, 0, 0,
                              GetTextureCoordinate(it->second.rotation_mode)));
  }
  CHECK_GL(glVertexAttribPointer(filter_position_attribute_, 2, GL_FLOAT, 0, 0,
                                 image_vertices));
  CHECK_GL(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));

  framebuffer_->Deactivate();

  return Source::DoRender(update_sinks);
}

const GLfloat* Filter::GetTextureCoordinate(
    const RotationMode& rotation_mode) const {
  static const GLfloat no_rotation_texture_coordinates[] = {
      0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
  };

  static const GLfloat rotate_left_texture_coordinates[] = {
      1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
  };

  static const GLfloat rotate_right_texture_coordinates[] = {
      0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f,
  };

  static const GLfloat vertical_flip_texture_coordinates[] = {
      0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
  };

  static const GLfloat horizontal_flip_texture_coordinates[] = {
      1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
  };

  static const GLfloat rotate_right_vertical_flip_texture_coordinates[] = {
      0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
  };

  static const GLfloat rotate_right_horizontal_flip_texture_coordinates[] = {
      1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
  };

  static const GLfloat rotate_180_texture_coordinates[] = {
      1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
  };

  switch (rotation_mode) {
    case NoRotation:
      return no_rotation_texture_coordinates;
      break;
    case RotateLeft:
      return rotate_left_texture_coordinates;
      break;
    case RotateRight:
      return rotate_right_texture_coordinates;
      break;
    case FlipVertical:
      return vertical_flip_texture_coordinates;
      break;
    case FlipHorizontal:
      return horizontal_flip_texture_coordinates;
      break;
    case RotateRightFlipVertical:
      return rotate_right_vertical_flip_texture_coordinates;
      break;
    case RotateRightFlipHorizontal:
      return rotate_right_horizontal_flip_texture_coordinates;
      break;
    case Rotate180:
      return rotate_180_texture_coordinates;
      break;
    default:
      break;
  }
}

void Filter::Render() {
  if (input_framebuffers_.empty()) {
    return;
  }

  if (GPUPixelContext::GetInstance()->is_capturing_frame_ &&
      this == GPUPixelContext::GetInstance()->capture_frame_filter_.get()) {
    int capture_width = GPUPixelContext::GetInstance()->capture_width_;
    int capture_height = GPUPixelContext::GetInstance()->capture_height_;

    if (!framebuffer_ || (framebuffer_->GetWidth() != capture_width ||
                          framebuffer_->GetHeight() != capture_height)) {
      framebuffer_ = GPUPixelContext::GetInstance()
                         ->GetFramebufferFactory()
                         ->CreateFramebuffer(capture_width, capture_height);
    }

    DoRender(false);

    framebuffer_->Activate();
    GPUPixelContext::GetInstance()->capture_frame_data_ =
        new unsigned char[capture_width * capture_height * 4];
    CHECK_GL(glReadPixels(0, 0, capture_width, capture_height, GL_RGBA,
                          GL_UNSIGNED_BYTE,
                          GPUPixelContext::GetInstance()->capture_frame_data_));
    framebuffer_->Deactivate();
  } else {
    std::shared_ptr<GPUPixelFramebuffer> first_input_framebuffer =
        input_framebuffers_.begin()->second.frame_buffer;
    RotationMode first_input_rotation =
        input_framebuffers_.begin()->second.rotation_mode;
    if (!first_input_framebuffer) {
      return;
    }

    int rotated_framebuffer_width = first_input_framebuffer->GetWidth();
    int rotated_framebuffer_height = first_input_framebuffer->GetHeight();
    if (rotationSwapsSize(first_input_rotation)) {
      rotated_framebuffer_width = first_input_framebuffer->GetHeight();
      rotated_framebuffer_height = first_input_framebuffer->GetWidth();
    }

    if (framebuffer_scale_ != 1.0) {
      rotated_framebuffer_width =
          int(rotated_framebuffer_width * framebuffer_scale_);
      rotated_framebuffer_height =
          int(rotated_framebuffer_height * framebuffer_scale_);
    }
    if (!framebuffer_ ||
        (framebuffer_->GetWidth() != rotated_framebuffer_width ||
         framebuffer_->GetHeight() != rotated_framebuffer_height)) {
      framebuffer_ = GPUPixelContext::GetInstance()
                         ->GetFramebufferFactory()
                         ->CreateFramebuffer(rotated_framebuffer_width,
                                             rotated_framebuffer_height);
    }
    DoRender(true);
  }
}

bool Filter::RegisterProperty(const std::string& name,
                              int default_value,
                              const std::string& comment /* = ""*/,
                              std::function<void(int&)> set_callback /* = 0*/) {
  if (HasProperty(name)) {
    return false;
  }
  IntProperty property;
  property.type = "int";
  property.value = default_value;
  property.comment = comment;
  property.set_callback = set_callback;
  int_properties_[name] = property;
  return true;
}

bool Filter::RegisterProperty(
    const std::string& name,
    float default_value,
    const std::string& comment /* = ""*/,
    std::function<void(float&)> set_callback /* = 0*/) {
  if (HasProperty(name)) {
    return false;
  }
  FloatProperty property;
  property.type = "float";
  property.value = default_value;
  property.comment = comment;
  property.set_callback = set_callback;
  float_properties_[name] = property;
  return true;
}

bool Filter::RegisterProperty(
    const std::string& name,
    std::vector<float> default_value,
    const std::string& comment /* = ""*/,
    std::function<void(std::vector<float>&)> set_callback /* = 0*/) {
  if (HasProperty(name)) {
    return false;
  }
  VectorProperty property;
  property.type = "vector";
  property.value = default_value;
  property.comment = comment;
  property.set_callback = set_callback;
  vector_properties_[name] = property;
  return true;
}

bool Filter::RegisterProperty(
    const std::string& name,
    const std::string& default_value,
    const std::string& comment /* = ""*/,
    std::function<void(std::string&)> set_callback /* = 0*/) {
  if (HasProperty(name)) {
    return false;
  }
  StringProperty property;
  property.type = "string";
  property.value = default_value;
  property.comment = comment;
  property.set_callback = set_callback;
  string_properties_[name] = property;
  return true;
}

bool Filter::SetProperty(const std::string& name, int value) {
  Property* raw_property = GetProperty(name);
  if (!raw_property) {
    Util::Log("WARNING", "Filter::setProperty invalid property %s",
              name.c_str());
    return false;
  } else if (raw_property->type != "int") {
    Util::Log("WARNING",
              "Filter::setProperty The property type is expected to be %s",
              raw_property->type.c_str());
    return false;
  }
  IntProperty* property = ((IntProperty*)raw_property);
  property->value = value;
  if (property->set_callback) {
    property->set_callback(value);
  }
  return true;
}

bool Filter::SetProperty(const std::string& name, float value) {
  Property* raw_property = GetProperty(name);
  if (!raw_property) {
    Util::Log("WARNING", "Filter::setProperty invalid property %s",
              name.c_str());
    return false;
  } else if (raw_property->type != "float") {
    Util::Log("WARNING",
              "Filter::setProperty The property type is expected to be %s",
              raw_property->type.c_str());
    return false;
  }
  FloatProperty* property = ((FloatProperty*)raw_property);
  if (property->set_callback) {
    property->set_callback(value);
  }
  property->value = value;

  return true;
}

bool Filter::SetProperty(const std::string& name, std::vector<float> value) {
  Property* raw_property = GetProperty(name);
  if (!raw_property) {
    Util::Log("WARNING", "Filter::setProperty invalid property %s",
              name.c_str());
    return false;
  } else if (raw_property->type != "vector") {
    Util::Log("WARNING",
              "Filter::setProperty The property type is expected to be %s",
              raw_property->type.c_str());
    return false;
  }
  VectorProperty* property = ((VectorProperty*)raw_property);
  if (property->set_callback) {
    property->set_callback(value);
  }
  property->value = value;

  return true;
}

bool Filter::SetProperty(const std::string& name, std::string value) {
  Property* raw_property = GetProperty(name);
  if (!raw_property) {
    Util::Log("WARNING", "Filter::setProperty invalid property %s",
              name.c_str());
    return false;
  } else if (raw_property->type != "string") {
    Util::Log("WARNING",
              "Filter::setProperty The property type is expected to be %s",
              raw_property->type.c_str());
    return false;
  }
  StringProperty* property = ((StringProperty*)raw_property);
  property->value = value;
  if (property->set_callback) {
    property->set_callback(value);
  }
  return true;
}

bool Filter::GetProperty(const std::string& name, int& ret_value) {
  Property* property = GetProperty(name);
  if (!property) {
    return false;
  }
  ret_value = ((IntProperty*)property)->value;
  return true;
}

bool Filter::GetProperty(const std::string& name, float& ret_value) {
  Property* property = GetProperty(name);
  if (!property) {
    return false;
  }
  ret_value = ((FloatProperty*)property)->value;
  return true;
}

bool Filter::GetProperty(const std::string& name, std::string& ret_value) {
  Property* property = GetProperty(name);
  if (!property) {
    return false;
  }
  ret_value = ((StringProperty*)property)->value;
  return true;
}

Filter::Property* Filter::GetProperty(const std::string& name) {
  if (int_properties_.find(name) != int_properties_.end()) {
    return &int_properties_[name];
  }
  if (float_properties_.find(name) != float_properties_.end()) {
    return &float_properties_[name];
  }
  if (vector_properties_.find(name) != vector_properties_.end()) {
    return &vector_properties_[name];
  }
  if (string_properties_.find(name) != string_properties_.end()) {
    return &string_properties_[name];
  }
  return 0;
}

bool Filter::HasProperty(const std::string& name, const std::string type) {
  Property* property = GetProperty(name);
  return property && property->type == type ? true : false;
}

bool Filter::HasProperty(const std::string& name) {
  return GetProperty(name) ? true : false;
}

bool Filter::GetPropertyComment(const std::string& name,
                                std::string& ret_comment) {
  Property* property = GetProperty(name);
  if (!property) {
    return false;
  }
  ret_comment = std::string("[") + property->type + "] " + property->comment;
  return true;
}

bool Filter::GetPropertyType(const std::string& name, std::string& ret_type) {
  Property* property = GetProperty(name);
  if (!property) {
    return false;
  }
  ret_type = property->type;
  return true;
}

}  // namespace gpupixel
