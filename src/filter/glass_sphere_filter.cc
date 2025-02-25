/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "glass_sphere_filter.h"

USING_NS_GPUPIXEL

REGISTER_FILTER_CLASS(GlassSphereFilter)

#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_ANDROID)
const std::string kGlassSphereFragmentShaderString = R"(

    uniform sampler2D inputImageTexture; varying highp vec2 textureCoordinate;

    uniform highp vec2 center;
    uniform highp float radius;
    uniform highp float aspectRatio;
    uniform highp float refractiveIndex;

    const highp vec3 lightPosition = vec3(-0.5, 0.5, 1.0);
    const highp vec3 ambientLightPosition = vec3(0.0, 0.0, 1.0);

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
          2.0 * refract(vec3(0.0, 0.0, -1.0), sphereNormal, refractiveIndex);
      refractedVector.xy = -refractedVector.xy;

      highp vec3 finalSphereColor =
          texture2D(inputImageTexture,
                    (refractedVector.xy + 1.0) * 0.5 + center - vec2(0.5))
              .rgb;

      // Grazing angle lighting
      highp float lightingIntensity =
          2.5 *
          (1.0 -
           pow(clamp(dot(ambientLightPosition, sphereNormal), 0.0, 1.0), 0.25));
      finalSphereColor += lightingIntensity;

      // Specular lighting
      // lightingIntensity  = clamp(dot(normalize(lightPosition), sphereNormal),
      // 0.0, 1.0); lightingIntensity  = pow(lightingIntensity, 15.0);
      // finalSphereColor += vec3(0.8, 0.8, 0.8) * lightingIntensity;

      gl_FragColor = vec4(finalSphereColor, 1.0) * checkForPresenceWithinSphere;
    })";
#elif defined(GPUPIXEL_MAC) || defined(GPUPIXEL_WIN) || defined(GPUPIXEL_LINUX)
const std::string kGlassSphereFragmentShaderString = R"(
    precision mediump float;
    uniform sampler2D inputImageTexture; varying vec2 textureCoordinate;

    uniform vec2 center;
    uniform float radius;
    uniform float aspectRatio;
    uniform float refractiveIndex;

    const vec3 lightPosition = vec3(-0.5, 0.5, 1.0);
    const vec3 ambientLightPosition = vec3(0.0, 0.0, 1.0);

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
          2.0 * refract(vec3(0.0, 0.0, -1.0), sphereNormal, refractiveIndex);
      refractedVector.xy = -refractedVector.xy;

      vec3 finalSphereColor =
          texture2D(inputImageTexture,
                    (refractedVector.xy + 1.0) * 0.5 + center - vec2(0.5))
              .rgb;

      // Grazing angle lighting
      float lightingIntensity =
          2.5 *
          (1.0 -
           pow(clamp(dot(ambientLightPosition, sphereNormal), 0.0, 1.0), 0.25));
      finalSphereColor += lightingIntensity;

      // Specular lighting
      // lightingIntensity  = clamp(dot(normalize(lightPosition), sphereNormal),
      // 0.0, 1.0); lightingIntensity  = pow(lightingIntensity, 15.0);
      // finalSphereColor += vec3(0.8, 0.8, 0.8) * lightingIntensity;

      gl_FragColor = vec4(finalSphereColor, 1.0) * checkForPresenceWithinSphere;
    })";

#endif

std::shared_ptr<GlassSphereFilter> GlassSphereFilter::create() {
  auto ret = std::shared_ptr<GlassSphereFilter>(new GlassSphereFilter());
  if (ret && !ret->init()) {
    ret.reset();
  }
  return ret;
}

bool GlassSphereFilter::init() {
  if (!initWithFragmentShaderString(kGlassSphereFragmentShaderString)) {
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
