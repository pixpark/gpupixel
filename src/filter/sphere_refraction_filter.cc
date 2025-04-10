/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "gpupixel/filter/sphere_refraction_filter.h"
#include "core/gpupixel_context.h"
namespace gpupixel {

#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_ANDROID)
const std::string kSphereRefractionShaderString = R"(

    uniform sampler2D inputImageTexture; uniform highp vec2 center;
    uniform highp float radius;
    uniform highp float aspectRatio;
    uniform highp float refractiveIndex;

    varying highp vec2 textureCoordinate;

    void main() {
      highp vec2 textureCoordinateToUse =
          vec2(textureCoordinate.x,
               (textureCoordinate.y * aspectRatio + 0.5 - 0.5 * aspectRatio));
      highp float distanceFromCenter = distance(center, textureCoordinateToUse);
      lowp float checkForPresenceWithinSphere =
          step(distanceFromCenter, radius);

      distanceFromCenter = distanceFromCenter / radius;

      highp float normalizedDepth =
          radius * sqrt(1.0 - distanceFromCenter * distanceFromCenter);
      highp vec3 sphereNormal =
          normalize(vec3(textureCoordinateToUse - center, normalizedDepth));

      highp vec3 refractedVector =
          refract(vec3(0.0, 0.0, -1.0), sphereNormal, refractiveIndex);

      gl_FragColor =
          texture2D(inputImageTexture, (refractedVector.xy + 1.0) * 0.5) *
          checkForPresenceWithinSphere;
    }

)";
#elif defined(GPUPIXEL_MAC) || defined(GPUPIXEL_WIN) || defined(GPUPIXEL_LINUX)
const std::string kSphereRefractionShaderString = R"(

    uniform sampler2D inputImageTexture; uniform vec2 center;
    uniform float radius;
    uniform float aspectRatio;
    uniform float refractiveIndex;

    varying vec2 textureCoordinate;

    void main() {
      vec2 textureCoordinateToUse =
          vec2(textureCoordinate.x,
               (textureCoordinate.y * aspectRatio + 0.5 - 0.5 * aspectRatio));
      float distanceFromCenter = distance(center, textureCoordinateToUse);
      float checkForPresenceWithinSphere = step(distanceFromCenter, radius);

      distanceFromCenter = distanceFromCenter / radius;

      float normalizedDepth =
          radius * sqrt(1.0 - distanceFromCenter * distanceFromCenter);
      vec3 sphereNormal =
          normalize(vec3(textureCoordinateToUse - center, normalizedDepth));

      vec3 refractedVector =
          refract(vec3(0.0, 0.0, -1.0), sphereNormal, refractiveIndex);

      gl_FragColor =
          texture2D(inputImageTexture, (refractedVector.xy + 1.0) * 0.5) *
          checkForPresenceWithinSphere;
    }

)";
#endif

std::shared_ptr<SphereRefractionFilter> SphereRefractionFilter::Create() {
  auto ret =
      std::shared_ptr<SphereRefractionFilter>(new SphereRefractionFilter());
  gpupixel::GPUPixelContext::GetInstance()->SyncRunWithContext([&] {
    if (ret && !ret->Init()) {
      ret.reset();
    }
  });
  return ret;
}

bool SphereRefractionFilter::Init() {
  if (!InitWithFragmentShaderString(kSphereRefractionShaderString)) {
    return false;
  }

  setPositionX(0.5);
  RegisterProperty("positionX", position_.x,
                   "The position of x about which to apply the distortion, "
                   "with a default of 0.5",
                   [this](float& positionX) { setPositionX(positionX); });

  setPositionY(0.5);
  RegisterProperty("positionY", position_.y,
                   "The position of y about which to apply the distortion, "
                   "with a default of 0.5",
                   [this](float& positionY) { setPositionY(positionY); });

  SetRadius(0.25);
  RegisterProperty("radius", radius_,
                   "The radius of the distortion, ranging from 0.0 to 1.0, "
                   "with a default of 0.25",
                   [this](float& radius) { SetRadius(radius); });

  setRefractiveIndex(0.71);
  RegisterProperty(
      "refractiveIndex", refractive_index_,
      "The index of refraction for the sphere, with a default of 0.71",
      [this](float& refractiveIndex) { setRefractiveIndex(refractiveIndex); });

  return true;
}

bool SphereRefractionFilter::DoRender(bool updateSinks) {
  filter_program_->SetUniformValue("center", position_);
  filter_program_->SetUniformValue("radius", radius_);
  filter_program_->SetUniformValue("refractiveIndex", refractive_index_);

  float aspectRatio = 1.0;
  std::shared_ptr<GPUPixelFramebuffer> firstInputFramebuffer =
      input_framebuffers_.begin()->second.frame_buffer;
  aspectRatio = firstInputFramebuffer->GetHeight() /
                (float)(firstInputFramebuffer->GetWidth());
  filter_program_->SetUniformValue("aspectRatio", aspectRatio);

  return Filter::DoRender(updateSinks);
}

void SphereRefractionFilter::setPositionX(float x) {
  position_.x = x;
}

void SphereRefractionFilter::setPositionY(float y) {
  position_.y = y;
}

void SphereRefractionFilter::SetRadius(float radius) {
  radius_ = radius;
}

void SphereRefractionFilter::setRefractiveIndex(float refractiveIndex) {
  refractive_index_ = refractiveIndex;
}

}  // namespace gpupixel
