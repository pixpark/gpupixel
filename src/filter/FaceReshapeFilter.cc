/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "FaceReshapeFilter.h"
#include "GPUPixelContext.h"

NS_GPUPIXEL_BEGIN

const std::string kGPUPixelThinFaceFragmentShaderString = SHADER_STRING
(
 precision highp float;
 varying highp vec2 textureCoordinate;
 uniform sampler2D inputImageTexture;

 uniform int hasFace;
 uniform float facePoints[106 * 2];

 uniform highp float aspectRatio;
 uniform float thinFaceDelta;
 uniform float bigEyeDelta;
 
 vec2 enlargeEye(vec2 textureCoord, vec2 originPosition, float radius, float delta) {

     float weight = distance(vec2(textureCoord.x, textureCoord.y / aspectRatio), vec2(originPosition.x, originPosition.y / aspectRatio)) / radius;

     weight = 1.0 - (1.0 - weight * weight) * delta;
     weight = clamp(weight,0.0,1.0);
     textureCoord = originPosition + (textureCoord - originPosition) * weight;
     return textureCoord;
 }
 
 vec2 curveWarp(vec2 textureCoord, vec2 originPosition, vec2 targetPosition, float delta) {

     vec2 offset = vec2(0.0);
     vec2 result = vec2(0.0);
     vec2 direction = (targetPosition - originPosition) * delta;

     float radius = distance(vec2(targetPosition.x, targetPosition.y / aspectRatio), vec2(originPosition.x, originPosition.y / aspectRatio));
     float ratio = distance(vec2(textureCoord.x, textureCoord.y / aspectRatio), vec2(originPosition.x, originPosition.y / aspectRatio)) / radius;

     ratio = 1.0 - ratio;
     ratio = clamp(ratio, 0.0, 1.0);
     offset = direction * ratio;

     result = textureCoord - offset;

     return result;
 }

 vec2 thinFace(vec2 currentCoordinate) {

     vec2 faceIndexs[9];
     faceIndexs[0] = vec2(3., 44.);
     faceIndexs[1] = vec2(29., 44.);
     faceIndexs[2] = vec2(7., 45.);
     faceIndexs[3] = vec2(25., 45.);
     faceIndexs[4] = vec2(10., 46.);
     faceIndexs[5] = vec2(22., 46.);
     faceIndexs[6] = vec2(14., 49.);
     faceIndexs[7] = vec2(18., 49.);
     faceIndexs[8] = vec2(16., 49.);

     for(int i = 0; i < 9; i++)
     {
         int originIndex = int(faceIndexs[i].x);
         int targetIndex = int(faceIndexs[i].y);
         vec2 originPoint = vec2(facePoints[originIndex * 2], facePoints[originIndex * 2 + 1]);
         vec2 targetPoint = vec2(facePoints[targetIndex * 2], facePoints[targetIndex * 2 + 1]);
         currentCoordinate = curveWarp(currentCoordinate, originPoint, targetPoint, thinFaceDelta);
     }
     return currentCoordinate;
 }

 vec2 bigEye(vec2 currentCoordinate) {

     vec2 faceIndexs[2];
     faceIndexs[0] = vec2(74., 72.);
     faceIndexs[1] = vec2(77., 75.);

     for(int i = 0; i < 2; i++)
     {
         int originIndex = int(faceIndexs[i].x);
         int targetIndex = int(faceIndexs[i].y);

         vec2 originPoint = vec2(facePoints[originIndex * 2], facePoints[originIndex * 2 + 1]);
         vec2 targetPoint = vec2(facePoints[targetIndex * 2], facePoints[targetIndex * 2 + 1]);

         float radius = distance(vec2(targetPoint.x, targetPoint.y / aspectRatio), vec2(originPoint.x, originPoint.y / aspectRatio));
         radius = radius * 5.;
         currentCoordinate = enlargeEye(currentCoordinate, originPoint, radius, bigEyeDelta);
     }
     return currentCoordinate;
 }

 void main()
 {
     vec2 positionToUse = textureCoordinate;

     if (hasFace == 1) {
         positionToUse = thinFace(positionToUse);
         positionToUse = bigEye(positionToUse);
     }

     gl_FragColor = texture2D(inputImageTexture, positionToUse);

 }
 );

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
    registerProperty("thin_face", 0, "The smoothing of filter with range between -1 and 1.", [this](float& val) {
        setFaceSlimLevel(val);
    });

    registerProperty("big_eye", 0, "The smoothing of filter with range between -1 and 1.", [this](float& val) {
        setEyeZoomLevel(val);
    });

  this->thinFaceDelta_ = 0.0;
  // [0, 0.15]
  this->bigEyeDelta_ = 0.0;
  return true;
}

void FaceReshapeFilter::setLandmarks(const std::vector<float> landmarks) {
  if (landmarks.size() == 0) {
    return;
  }
  face_land_marks_ = landmarks;
}

void FaceReshapeFilter::setHasFace(bool has_face) {
  has_face_ = has_face ? 1 : 0;
}

bool FaceReshapeFilter::proceed(bool bUpdateTargets, int64_t frameTime) {
  float aspect = (float)_framebuffer->getWidth() / _framebuffer->getHeight();
  _filterProgram->setUniformValue("aspectRatio", aspect);

  _filterProgram->setUniformValue("thinFaceDelta", this->thinFaceDelta_);

  _filterProgram->setUniformValue("bigEyeDelta", this->bigEyeDelta_);

  _filterProgram->setUniformValue("hasFace", has_face_);
  if (has_face_) {
    _filterProgram->setUniformValue("facePoints", face_land_marks_.data(),
                                    static_cast<int>(face_land_marks_.size()));
  }
  return Filter::proceed(bUpdateTargets, frameTime);
}

#pragma mark - face slim
void FaceReshapeFilter::setFaceSlimLevel(float level) {
  thinFaceDelta_ = level;
}

#pragma mark - eye zoom
void FaceReshapeFilter::setEyeZoomLevel(float level) {
  bigEyeDelta_ = level;
}

NS_GPUPIXEL_END
