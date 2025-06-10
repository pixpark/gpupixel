/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "gpupixel/filter/face_reshape_filter.h"
#include "core/gpupixel_context.h"

namespace gpupixel {

#if defined(GPUPIXEL_GLES_SHADER)
const std::string kGPUPixelThinFaceFragmentShaderString = R"(
 precision highp float;
 varying highp vec2 textureCoordinate;
 uniform sampler2D inputImageTexture;

 uniform int faceCount; 
 uniform float facePoints[4*222]; 

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

 vec2 thinFace(vec2 currentCoordinate, int faceIndex) {
     int baseIndex = faceIndex * 222;

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
         vec2 originPoint = vec2(facePoints[baseIndex + originIndex * 2], facePoints[baseIndex + originIndex * 2 + 1]);
         vec2 targetPoint = vec2(facePoints[baseIndex + targetIndex * 2], facePoints[baseIndex + targetIndex * 2 + 1]);
         currentCoordinate = curveWarp(currentCoordinate, originPoint, targetPoint, thinFaceDelta);
     }
     return currentCoordinate;
 }

 vec2 bigEye(vec2 currentCoordinate, int faceIndex) {
     int baseIndex = faceIndex * 222;

     vec2 faceIndexs[2];
     faceIndexs[0] = vec2(74., 72.);
     faceIndexs[1] = vec2(77., 75.);

     for(int i = 0; i < 2; i++)
     {
         int originIndex = int(faceIndexs[i].x);
         int targetIndex = int(faceIndexs[i].y);

         vec2 originPoint = vec2(facePoints[baseIndex + originIndex * 2], facePoints[baseIndex + originIndex * 2 + 1]);
         vec2 targetPoint = vec2(facePoints[baseIndex + targetIndex * 2], facePoints[baseIndex + targetIndex * 2 + 1]);

         float radius = distance(vec2(targetPoint.x, targetPoint.y / aspectRatio), vec2(originPoint.x, originPoint.y / aspectRatio));
         radius = radius * 5.;
         currentCoordinate = enlargeEye(currentCoordinate, originPoint, radius, bigEyeDelta);
     }
     return currentCoordinate;
 }

 void main()
 {
     vec2 positionToUse = textureCoordinate;

     for(int i = 0; i < faceCount; i++) {
         positionToUse = thinFace(positionToUse, i);
         positionToUse = bigEye(positionToUse, i);
     }

     gl_FragColor = texture2D(inputImageTexture, positionToUse);
 }
 )";
#elif defined(GPUPIXEL_GL_SHADER)
const std::string kGPUPixelThinFaceFragmentShaderString = R"(
 varying vec2 textureCoordinate;
 uniform sampler2D inputImageTexture;

 uniform int faceCount; 
 uniform float facePoints[4*222]; 
 uniform float aspectRatio;
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

 vec2 thinFace(vec2 currentCoordinate, int faceIndex) {
     int baseIndex = faceIndex * 222;
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
         vec2 originPoint = vec2(facePoints[baseIndex + originIndex * 2], facePoints[baseIndex + originIndex * 2 + 1]);
         vec2 targetPoint = vec2(facePoints[baseIndex + targetIndex * 2], facePoints[baseIndex + targetIndex * 2 + 1]);
         currentCoordinate = curveWarp(currentCoordinate, originPoint, targetPoint, thinFaceDelta);
     }
     return currentCoordinate;
 }

 vec2 bigEye(vec2 currentCoordinate, int faceIndex) {
     int baseIndex = faceIndex * 222;

     vec2 faceIndexs[2];
     faceIndexs[0] = vec2(74., 72.);
     faceIndexs[1] = vec2(77., 75.);

     for(int i = 0; i < 2; i++)
     {
         int originIndex = int(faceIndexs[i].x);
         int targetIndex = int(faceIndexs[i].y);

         vec2 originPoint = vec2(facePoints[baseIndex + originIndex * 2], facePoints[baseIndex + originIndex * 2 + 1]);
         vec2 targetPoint = vec2(facePoints[baseIndex + targetIndex * 2], facePoints[baseIndex + targetIndex * 2 + 1]);

         float radius = distance(vec2(targetPoint.x, targetPoint.y / aspectRatio), vec2(originPoint.x, originPoint.y / aspectRatio));
         radius = radius * 5.;
         currentCoordinate = enlargeEye(currentCoordinate, originPoint, radius, bigEyeDelta);
     }
     return currentCoordinate;
 }

 void main()
 {
     vec2 positionToUse = textureCoordinate;

     for(int i = 0; i < faceCount; i++) {
         positionToUse = thinFace(positionToUse, i);
         positionToUse = bigEye(positionToUse, i);
     }

     gl_FragColor = texture2D(inputImageTexture, positionToUse);
 }
 )";
#endif

FaceReshapeFilter::FaceReshapeFilter() {}

FaceReshapeFilter::~FaceReshapeFilter() {}

std::shared_ptr<FaceReshapeFilter> FaceReshapeFilter::Create() {
  auto ret = std::shared_ptr<FaceReshapeFilter>(new FaceReshapeFilter());
  gpupixel::GPUPixelContext::GetInstance()->SyncRunWithContext([&] {
    if (ret && !ret->Init()) {
      ret.reset();
    }
  });
  return ret;
}

bool FaceReshapeFilter::Init() {
  if (!InitWithFragmentShaderString(kGPUPixelThinFaceFragmentShaderString)) {
    return false;
  }
  RegisterProperty("thin_face", 0,
                   "The smoothing of filter with range between -1 and 1.",
                   [this](float& val) { SetFaceSlimLevel(val); });

  RegisterProperty("big_eye", 0,
                   "The smoothing of filter with range between -1 and 1.",
                   [this](float& val) { SetEyeZoomLevel(val); });

  std::vector<float> defaut;
  RegisterProperty("face_landmark", defaut,
                   "The face landmark of filter with range between -1 and 1.",
                   [this](std::vector<float> val) { SetFaceLandmarks(val); });

  this->thin_face_delta_ = 0.0;
  // [0, 0.15]
  this->big_eye_delta_ = 0.0;
  return true;
}

// Process landmarks for single or multiple faces (each face has 478 points with
// x,y coordinates)
void FaceReshapeFilter::SetFaceLandmarks(std::vector<float> landmarks) {
  face_landmarks_.clear();

  if (landmarks.empty()) {
    return;
  }
  face_landmarks_ = landmarks;
  face_count_ = static_cast<int>(face_landmarks_.size() / 222);
}

// Render the filter with current face landmarks and effect parameters
bool FaceReshapeFilter::DoRender(bool updateSinks) {
  float aspect = (float)framebuffer_->GetWidth() / framebuffer_->GetHeight();
  filter_program_->SetUniformValue("aspectRatio", aspect);
  filter_program_->SetUniformValue("thinFaceDelta", thin_face_delta_);
  filter_program_->SetUniformValue("bigEyeDelta", big_eye_delta_);

  filter_program_->SetUniformValue("faceCount", face_count_);

  if (!face_landmarks_.empty()) {
    filter_program_->SetUniformValue("facePoints", face_landmarks_.data(),
                                     static_cast<int>(face_landmarks_.size()));
  }

  return Filter::DoRender(updateSinks);
}

#pragma mark - face slim
void FaceReshapeFilter::SetFaceSlimLevel(float level) {
  thin_face_delta_ = std::clamp(level, -1.0f, 1.0f);
}

#pragma mark - eye zoom
void FaceReshapeFilter::SetEyeZoomLevel(float level) {
  // Recommended range: [0, 0.15]
  big_eye_delta_ = std::clamp(level, -1.0f, 1.0f);
}

}  // namespace gpupixel
