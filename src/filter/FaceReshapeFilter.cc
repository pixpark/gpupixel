/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "FaceReshapeFilter.h"
#include "GPUPixelContext.h"

NS_GPUPIXEL_BEGIN

const std::string kGPUPixelThinFaceFragmentShaderString = SHADER_STRING(
    precision highp float; varying NEBULA_HIGHP vec2 textureCoordinate;
    uniform NEBULA_HIGHP float aspectRatio;
    uniform sampler2D inputImageTexture;

    uniform int hasFace;
    uniform float facePoints[106 * 2];

    // face
    uniform float thinFaceDelta;
    // eye
    uniform float bigEyeDelta;

    vec2 warpEyes(vec2 originPosition,
                  vec2 textureCoord,
                  float radius,
                  float scaleRatio) {
      vec2 result = textureCoord;
      float d =
          distance(vec2(textureCoord.x, textureCoord.y / aspectRatio),
                   vec2(originPosition.x, originPosition.y / aspectRatio));

      if (d < radius) {
        float gamma =
            1.0 - scaleRatio * pow(smoothstep(0.0, 1.0, d / radius) - 1.0, 2.0);
        result = originPosition + gamma * (textureCoord - originPosition);
      }
      return result;
    }

    vec2 curveWarp(vec2 textureCoord,
                   vec2 originPosition,
                   vec2 targetPosition,
                   float delta) {
      vec2 offset = vec2(0.0);
      vec2 result = vec2(0.0);
      vec2 direction = (targetPosition - originPosition) * delta;

      float radius =
          distance(vec2(targetPosition.x, targetPosition.y / aspectRatio),
                   vec2(originPosition.x, originPosition.y / aspectRatio));
      float ratio =
          distance(vec2(textureCoord.x, textureCoord.y / aspectRatio),
                   vec2(originPosition.x, originPosition.y / aspectRatio)) /
          radius;

      ratio = 1.0 - ratio;
      ratio = clamp(ratio, 0.0, 1.0);
      offset = direction * ratio;

      result = textureCoord - offset;

      return result;
    }

    vec2 thinFace(vec2 currentCoordinate) {
      vec2 faceIndexs[11];
      faceIndexs[0] = vec2(3., 54.);
      faceIndexs[1] = vec2(29., 54.);
      faceIndexs[2] = vec2(7., 53.);
      faceIndexs[3] = vec2(25., 53.);
      faceIndexs[4] = vec2(10., 52.);
      faceIndexs[5] = vec2(22., 52.);
      faceIndexs[6] = vec2(12., 51.);
      faceIndexs[7] = vec2(20., 51.);
      faceIndexs[8] = vec2(14., 63.);
      faceIndexs[9] = vec2(18., 57.);
      faceIndexs[10] = vec2(16., 99);

      for (int i = 0; i < 11; i++) {
        int originIndex = int(faceIndexs[i].x);
        int targetIndex = int(faceIndexs[i].y);
        vec2 originPoint =
            vec2(facePoints[originIndex * 2], facePoints[originIndex * 2 + 1]);
        vec2 targetPoint =
            vec2(facePoints[targetIndex * 2], facePoints[targetIndex * 2 + 1]);
        currentCoordinate = curveWarp(currentCoordinate, originPoint,
                                      targetPoint, thinFaceDelta);
      }
      return currentCoordinate;
    }

    vec2 bigEye(vec2 currentCoordinate) {
      vec2 faceIndexs[2];
      faceIndexs[0] = vec2(75, 76.);
      faceIndexs[1] = vec2(87, 88.);

      for (int i = 0; i < 2; i++) {
        int originIndex = int(faceIndexs[i].x);
        int targetIndex = int(faceIndexs[i].y);

        vec2 originPoint =
            vec2(facePoints[originIndex * 2], facePoints[originIndex * 2 + 1]);

        vec2 targetPoint = vec2(facePoints[73 * 2], facePoints[73 * 2 + 1]);

        vec2 destPoint = vec2(facePoints[69 * 2], facePoints[69 * 2 + 1]);
        float radius =
            distance(vec2(targetPoint.x, targetPoint.y / aspectRatio),
                     vec2(destPoint.x, destPoint.y / aspectRatio));

        radius = radius * 2.5;
        currentCoordinate =
            warpEyes(originPoint, currentCoordinate, radius, bigEyeDelta);
      }
      return currentCoordinate;
    }

    void main() {
      vec2 positionToUse = textureCoordinate;

      if (hasFace == 1) {
        // thinface
        if (thinFaceDelta > 0.0) {
          positionToUse = thinFace(positionToUse);
        }
        // big eye
        if (bigEyeDelta > 0.0) {
          positionToUse = bigEye(positionToUse);
        }
      }

      gl_FragColor = texture2D(inputImageTexture, positionToUse);
    });

FaceReshapeFilter::FaceReshapeFilter() {}

FaceReshapeFilter::~FaceReshapeFilter() {}

std::shared_ptr<FaceReshapeFilter> FaceReshapeFilter::create() {
  auto ret = std::shared_ptr<FaceReshapeFilter>(new FaceReshapeFilter());
  if (ret && !ret->init()) {
    ret.reset();
  }
  return ret;
}

bool FaceReshapeFilter::init() {
  if (!initWithFragmentShaderString(kGPUPixelThinFaceFragmentShaderString)) {
    return false;
  }

  this->thinFaceDelta_ = 0.0;
  // [0, 0.15]
  this->eyeZoomLevel_ = 0.0;
  return true;
}

void FaceReshapeFilter::setUniformWithLandmarks(const float* land_marks,
                                                int lenght) {
  if (lenght == 0) {
    return;
  }

  face_land_marks_ = std::vector<float>(land_marks, land_marks + lenght);
  ;
}

void FaceReshapeFilter::setHasFace(bool has_face) {
  has_face_ = has_face ? 1 : 0;
}

bool FaceReshapeFilter::proceed(bool bUpdateTargets, int64_t frameTime) {
  float aspect = (float)_framebuffer->getWidth() / _framebuffer->getHeight();
  _filterProgram->setUniformValue("aspectRatio", aspect);

  _filterProgram->setUniformValue("thinFaceDelta", this->thinFaceDelta_);

  _filterProgram->setUniformValue("bigEyeDelta", this->eyeZoomLevel_);

  _filterProgram->setUniformValue("hasFace", has_face_);
  if (has_face_) {
    _filterProgram->setUniformValue("facePoints", face_land_marks_.data(),
                                    face_land_marks_.size());
  }
  return Filter::proceed(bUpdateTargets, frameTime);
}

#pragma mark - face slim
void FaceReshapeFilter::setFaceSlimLevel(float level) {
  thinFaceDelta_ = level;
}

#pragma mark - eye zoom
void FaceReshapeFilter::setEyeZoomLevel(float level) {
  eyeZoomLevel_ = level;
}

NS_GPUPIXEL_END
