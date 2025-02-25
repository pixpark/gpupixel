/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "sphere_refraction_filter.h"

USING_NS_GPUPIXEL

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
    precision mediump float;
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

std::shared_ptr<SphereRefractionFilter> SphereRefractionFilter::create() {
  auto ret =
      std::shared_ptr<SphereRefractionFilter>(new SphereRefractionFilter());
  if (ret && !ret->init()) {
    ret.reset();
  }
  return ret;
}

bool SphereRefractionFilter::init() {
  if (!initWithFragmentShaderString(kSphereRefractionShaderString)) {
    return false;
  }

  setPositionX(0.5);
  registerProperty("positionX", _position.x,
                   "The position of x about which to apply the distortion, "
                   "with a default of 0.5",
                   [this](float& positionX) { setPositionX(positionX); });

  setPositionY(0.5);
  registerProperty("positionY", _position.y,
                   "The position of y about which to apply the distortion, "
                   "with a default of 0.5",
                   [this](float& positionY) { setPositionY(positionY); });

  setRadius(0.25);
  registerProperty("radius", _radius,
                   "The radius of the distortion, ranging from 0.0 to 1.0, "
                   "with a default of 0.25",
                   [this](float& radius) { setRadius(radius); });

  setRefractiveIndex(0.71);
  registerProperty(
      "refractiveIndex", _refractiveIndex,
      "The index of refraction for the sphere, with a default of 0.71",
      [this](float& refractiveIndex) { setRefractiveIndex(refractiveIndex); });

  return true;
}

bool SphereRefractionFilter::proceed(bool bUpdateTargets, int64_t frameTime) {
  _filterProgram->setUniformValue("center", _position);
  _filterProgram->setUniformValue("radius", _radius);
  _filterProgram->setUniformValue("refractiveIndex", _refractiveIndex);

  float aspectRatio = 1.0;
  std::shared_ptr<Framebuffer> firstInputFramebuffer =
      _inputFramebuffers.begin()->second.frameBuffer;
  aspectRatio = firstInputFramebuffer->getHeight() /
                (float)(firstInputFramebuffer->getWidth());
  _filterProgram->setUniformValue("aspectRatio", aspectRatio);

  return Filter::proceed(bUpdateTargets, frameTime);
}

void SphereRefractionFilter::setPositionX(float x) {
  _position.x = x;
}

void SphereRefractionFilter::setPositionY(float y) {
  _position.y = y;
}

void SphereRefractionFilter::setRadius(float radius) {
  _radius = radius;
}

void SphereRefractionFilter::setRefractiveIndex(float refractiveIndex) {
  _refractiveIndex = refractiveIndex;
}
