/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "FaceMakeupFilter.h"
#include "GPUPixelContext.h"
#include "source/SourceImage.h"

NS_GPUPIXEL_BEGIN

const std::string FaceMakeupFilterVertexShaderString = SHADER_STRING(
    attribute vec3 position; attribute vec2 inputTextureCoordinate;
    varying vec2 textureCoordinate;
    varying vec2 textureCoordinate2;

    void main(void) {
      gl_Position = vec4(position, 1.);
      textureCoordinate = inputTextureCoordinate;
      textureCoordinate2 = position.xy * 0.5 + 0.5;  // landmark
    });

const std::string FaceMakeupFilterFragmentShaderString = SHADER_STRING(
    precision mediump float; varying highp vec2 textureCoordinate;
    varying highp vec2 textureCoordinate2;
    uniform sampler2D inputImageTexture;
    uniform sampler2D inputImageTexture2;  // mouth

    uniform float intensity;
    uniform int blendMode;

    float blendHardLight(float base, float blend) {
      return blend < 0.5 ? (2.0 * base * blend)
                         : (1.0 - 2.0 * (1.0 - base) * (1.0 - blend));
    }

    vec3 blendHardLight(vec3 base, vec3 blend) {
      return vec3(blendHardLight(base.r, blend.r),
                  blendHardLight(base.g, blend.g),
                  blendHardLight(base.b, blend.b));
    }

    float blendSoftLight(float base, float blend) {
      return (blend < 0.5) ? (base + (2.0 * blend - 1.0) * (base - base * base))
                           : (base + (2.0 * blend - 1.0) * (sqrt(base) - base));
    } vec3 blendSoftLight(vec3 base, vec3 blend) {
      return vec3(blendSoftLight(base.r, blend.r),
                  blendSoftLight(base.g, blend.g),
                  blendSoftLight(base.b, blend.b));
    }

    vec3 blendMultiply(vec3 base, vec3 blend) { return base * blend; }

    float blendOverlay(float base, float blend) {
      return base < 0.5 ? (2.0 * base * blend)
                        : (1.0 - 2.0 * (1.0 - base) * (1.0 - blend));
    } vec3 blendOverlay(vec3 base, vec3 blend) {
      return vec3(blendOverlay(base.r, blend.r), blendOverlay(base.g, blend.g),
                  blendOverlay(base.b, blend.b));
    }

    vec3 blendFunc(vec3 base, vec3 blend, int blendMode) {
      if (blendMode == 0) {
        return blend;
      } else if (blendMode == 15) {
        return blendMultiply(base, blend);
      } else if (blendMode == 17) {
        return blendOverlay(base, blend);
      } else if (blendMode == 22) {
        return blendHardLight(base, blend);
      }
      return blend;
    }

    void main() {
      vec4 fgColor = texture2D(inputImageTexture2, textureCoordinate);  // mouth
      fgColor = fgColor * intensity;
      vec4 bgColor = texture2D(inputImageTexture, textureCoordinate2);
      if (fgColor.a == 0.0) {
        gl_FragColor = bgColor;
        return;
      }

      vec3 color = blendFunc(bgColor.rgb,
                             clamp(fgColor.rgb * (1.0 / fgColor.a), 0.0, 1.0),
                             blendMode);
      //    color = color * intensity;
      gl_FragColor =
          vec4(bgColor.rgb * (1.0 - fgColor.a) + color.rgb * fgColor.a, 1.0);
    });

FaceMakeupFilter::FaceMakeupFilter() {}

FaceMakeupFilter::~FaceMakeupFilter() {}

std::shared_ptr<FaceMakeupFilter> FaceMakeupFilter::create() {
  auto ret = std::shared_ptr<FaceMakeupFilter>(new FaceMakeupFilter());
  if (ret && !ret->init()) {
    ret.reset();
  }
  return ret;
}

bool FaceMakeupFilter::init() {
  if (!Filter::initWithShaderString(FaceMakeupFilterVertexShaderString,
                                    FaceMakeupFilterFragmentShaderString)) {
    return false;
  }

  // lipstick render program
  _filterPositionAttribute = _filterProgram->getAttribLocation("position");
  _filterTexCoordAttribute =
      _filterProgram->getAttribLocation("inputTextureCoordinate");

  // base render program
  _filterProgram2 = GLProgram::createByShaderString(kDefaultVertexShader,
                                                    kDefaultFragmentShader);
  _filterPositionAttribute2 = _filterProgram2->getAttribLocation("position");
  _filterTexCoordAttribute2 =
      _filterProgram2->getAttribLocation("inputTextureCoordinate");

  return true;
}

void FaceMakeupFilter::setFaceLandmarks(const float* land_marks, int lenght) {
  if (lenght == 0) {
    return;
  }
  std::vector<float> vect;
  for (int i = 0; i < lenght; i++) {
    vect.push_back(2 * land_marks[i] - 1);
  }
  // left face index: 110
  float x = (land_marks[4 * 2] + land_marks[56 * 2]) / 2;
  float y = (land_marks[4 * 2 + 1] + land_marks[56 * 2 + 1]) / 2;
  vect.push_back(2 * x - 1);
  vect.push_back(2 * y - 1);

  // right face index: 111
  x = (land_marks[64 * 2] + land_marks[28 * 2]) / 2;
  y = (land_marks[64 * 2 + 1] + land_marks[28 * 2 + 1]) / 2;
  vect.push_back(2 * x - 1);
  vect.push_back(2 * y - 1);

  face_land_marks_ = vect;
}

bool FaceMakeupFilter::proceed(bool bUpdateTargets, int64_t frameTime) {
  static const GLfloat imageVertices[] = {
    -1.0f, -1.0f,
    1.0f, -1.0f,
    -1.0f, 1.0f,
    1.0f, 1.0f,
  };

  _framebuffer->active();
  // render base frame --- begin -----//
  GPUPixelContext::getInstance()->setActiveShaderProgram(_filterProgram2);
  CHECK_GL(glClearColor(_backgroundColor.r, _backgroundColor.g,
                        _backgroundColor.b, _backgroundColor.a));
  CHECK_GL(glClear(GL_COLOR_BUFFER_BIT));

  CHECK_GL(glActiveTexture(GL_TEXTURE4));
  CHECK_GL(glBindTexture(GL_TEXTURE_2D,
                         _inputFramebuffers[0].frameBuffer->getTexture()));
  _filterProgram2->setUniformValue("inputImageTexture", 4);

  // vertex
  CHECK_GL(glEnableVertexAttribArray(_filterPositionAttribute2));
  CHECK_GL(glVertexAttribPointer(_filterPositionAttribute2, 2, GL_FLOAT, 0, 0,
                                 imageVertices));

  CHECK_GL(glEnableVertexAttribArray(_filterTexCoordAttribute2));
  CHECK_GL(glVertexAttribPointer(_filterTexCoordAttribute2, 2, GL_FLOAT, 0, 0,
                                 _getTexureCoordinate(NoRotation)));

  CHECK_GL(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));

  // render lipstick image --- begin --- //
  GPUPixelContext::getInstance()->setActiveShaderProgram(_filterProgram);

  CHECK_GL(glEnableVertexAttribArray(_filterPositionAttribute));
  if (face_land_marks_.size() != 0) {
    CHECK_GL(glVertexAttribPointer(_filterPositionAttribute, 2, GL_FLOAT, 0, 0,
                                   face_land_marks_.data()));
  }

  auto coord = this->faceTextureCoordinates();
  std::vector<GLfloat> textureCoordinates(coord.size());
  auto point_count = coord.size() / 2;
  for (int i = 0; i < point_count; i++) {
    textureCoordinates[i * 2 + 0] =
        (coord[i * 2 + 0] * 1280 - texture_bounds_.x) / texture_bounds_.width;
    textureCoordinates[i * 2 + 1] =
        (coord[i * 2 + 1] * 1280 - texture_bounds_.y) / texture_bounds_.height;
  }
  // texcoord attribute
  CHECK_GL(glEnableVertexAttribArray(_filterTexCoordAttribute));
  CHECK_GL(glVertexAttribPointer(_filterTexCoordAttribute, 2, GL_FLOAT, 0, 0,
                                 textureCoordinates.data()));

  _filterProgram->setUniformValue("intensity", this->blend_level_);

  _filterProgram->setUniformValue("blendMode", 15);

  std::shared_ptr<Framebuffer> fb = _inputFramebuffers[0].frameBuffer;
  CHECK_GL(glActiveTexture(GL_TEXTURE0));
  CHECK_GL(glBindTexture(GL_TEXTURE_2D, fb->getTexture()));
  _filterProgram->setUniformValue("inputImageTexture", 0);  // origin image

  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_2D, image_texture_->getFramebuffer()->getTexture());
  _filterProgram->setUniformValue("inputImageTexture2", 3);

  if (has_face_) {
    auto face_indexs = this->getFaceIndexs();
    glDrawElements(GL_TRIANGLES, (GLsizei)face_indexs.size(), GL_UNSIGNED_INT,
                   face_indexs.data());
  }
  _framebuffer->inactive();

  return Source::proceed(bUpdateTargets, frameTime);
}

std::vector<GLuint> FaceMakeupFilter::getFaceIndexs() {
  static std::vector<GLuint> faceIndexs{};
  return faceIndexs;
}

std::vector<GLfloat> FaceMakeupFilter::faceTextureCoordinates() {
  static std::vector<GLfloat> arr = {

  };
  return arr;
}
NS_GPUPIXEL_END
