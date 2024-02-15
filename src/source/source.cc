/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "source.h"
#include "gpupixel_context.h"
#include "util.h"

#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_MAC)
#include "objc_target.h"
#endif

NS_GPUPIXEL_BEGIN

Source::Source()
    : _framebuffer(0),
      _outputRotation(RotationMode::NoRotation),
      _framebufferScale(1.0) {}

Source::~Source() {
  removeAllTargets();
}

std::shared_ptr<Source> Source::addTarget(std::shared_ptr<Target> target) {
  int targetTexIdx = target->getNextAvailableTextureIndex();
  return addTarget(target, targetTexIdx);
}

std::shared_ptr<Source> Source::addTarget(std::shared_ptr<Target> target,
                                          int texIdx) {
  if (!hasTarget(target)) {
    _targets[target] = texIdx;
    target->setInputFramebuffer(_framebuffer, RotationMode::NoRotation, texIdx);
  }
  return std::dynamic_pointer_cast<Source>(target);
}

#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_MAC)
std::shared_ptr<Source> Source::addTarget(id<GPUPixelTarget> target) {
  auto ios_target = std::shared_ptr<Target>(new ObjcTarget(target));
  addTarget(ios_target);
  return 0;
}
#endif

bool Source::hasTarget(const std::shared_ptr<Target> target) const {
  if (_targets.find(target) != _targets.end()) {
    return true;
  } else {
    return false;
  }
}

void Source::removeTarget(std::shared_ptr<Target> target) {
  auto itr = _targets.find(target);
  if (itr != _targets.end()) {
    _targets.erase(itr);
  }
}

void Source::removeAllTargets() {
  _targets.clear();
}

bool Source::proceed(bool bUpdateTargets /* = true*/,
                     int64_t frameTime /* = 0*/) {
  if (bUpdateTargets) {
    updateTargets(frameTime);
  }
  return true;
}

void Source::updateTargets(int64_t frameTime) {
  for (auto& it : _targets) {
    auto target = it.first;
    target->setInputFramebuffer(_framebuffer, _outputRotation,
                                _targets[target]);
    if (target->isPrepared()) {
      target->update(frameTime);
      target->unPrepear();
    }
  }
}

unsigned char* Source::captureAProcessedFrameData(
    std::shared_ptr<Filter> upToFilter,
    int width /* = 0*/,
    int height /* = 0*/) {
  if (GPUPixelContext::getInstance()->isCapturingFrame) {
    return 0;
  }

  if (width <= 0 || height <= 0) {
    if (!_framebuffer) {
      return 0;
    }
    width = getRotatedFramebufferWidth();
    height = getRotatedFramebufferHeight();
  }

  GPUPixelContext::getInstance()->isCapturingFrame = true;
  GPUPixelContext::getInstance()->captureWidth = width;
  GPUPixelContext::getInstance()->captureHeight = height;
  GPUPixelContext::getInstance()->captureUpToFilter = upToFilter;

  proceed(true);
  unsigned char* processedFrameData =
      GPUPixelContext::getInstance()->capturedFrameData;

  GPUPixelContext::getInstance()->capturedFrameData = 0;
  GPUPixelContext::getInstance()->captureWidth = 0;
  GPUPixelContext::getInstance()->captureHeight = 0;
  GPUPixelContext::getInstance()->isCapturingFrame = false;

  return processedFrameData;
}

void Source::setFramebuffer(
    std::shared_ptr<Framebuffer> fb,
    RotationMode outputRotation /* = RotationMode::NoRotation*/) {
  _framebuffer = fb;
  _outputRotation = outputRotation;
}

int Source::getRotatedFramebufferWidth() const {
  if (_framebuffer) {
    if (rotationSwapsSize(_outputRotation)) {
      return _framebuffer->getHeight();
    } else {
      return _framebuffer->getWidth();
    }
  } else {
    return 0;
  }
}
 
int Source::RegLandmarkCallback(FaceDetectorCallback callback) {
  if(_face_detector == nullptr) {
    // init face detector
    _face_detector = std::make_shared<FaceDetector>();
  }

  _face_detector->RegCallback(callback);
}

int Source::getRotatedFramebufferHeight() const {
  if (_framebuffer) {
    if (rotationSwapsSize(_outputRotation)) {
      return _framebuffer->getWidth();
    } else {
      return _framebuffer->getHeight();
    }
  } else {
    return 0;
  }
}

std::shared_ptr<Framebuffer> Source::getFramebuffer() const {
  return _framebuffer;
}

void Source::releaseFramebuffer(bool returnToCache /* = true*/) {}

NS_GPUPIXEL_END
