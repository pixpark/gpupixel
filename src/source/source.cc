/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "source.h"
#include "gpupixel_context.h"
#include "util.h"

#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_MAC)
#include "objc_sink.h"
#endif

namespace gpupixel {

Source::Source()
    : _framebuffer(0),
      _outputRotation(RotationMode::NoRotation),
      _framebufferScale(1.0) {}

Source::~Source() {
  removeAllSinks();
}

std::shared_ptr<Source> Source::addSink(std::shared_ptr<Sink> sink) {
  int sinkTexIdx = sink->getNextAvailableTextureIndex();
  return addSink(sink, sinkTexIdx);
}

std::shared_ptr<Source> Source::addSink(std::shared_ptr<Sink> sink,
                                          int texIdx) {
  if (!hasSink(sink)) {
    _sinks[sink] = texIdx;
    sink->setInputFramebuffer(_framebuffer, RotationMode::NoRotation, texIdx);
  }
  return std::dynamic_pointer_cast<Source>(sink);
}

#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_MAC)
std::shared_ptr<Source> Source::addSink(id<GPUPixelSink> sink) {
  auto ios_sink = std::shared_ptr<Sink>(new ObjcSink(sink));
  addSink(ios_sink);
  return 0;
}
#endif

bool Source::hasSink(const std::shared_ptr<Sink> sink) const {
  if (_sinks.find(sink) != _sinks.end()) {
    return true;
  } else {
    return false;
  }
}

void Source::removeSink(std::shared_ptr<Sink> sink) {
  auto itr = _sinks.find(sink);
  if (itr != _sinks.end()) {
    _sinks.erase(itr);
  }
}

void Source::removeAllSinks() {
  _sinks.clear();
}

bool Source::doRender(bool updateSinks) {
  if (updateSinks) {
    doUpdateSinks();
  }
  return true;
}

void Source::doUpdateSinks() {
  for (auto& it : _sinks) {
    auto sink = it.first;
    sink->setInputFramebuffer(_framebuffer, _outputRotation,
                                _sinks[sink]);
    if (sink->isPrepared()) {
      sink->render();
      sink->unPrepear();
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

  doRender(true);
  unsigned char* processedFrameData =
      GPUPixelContext::getInstance()->capturedFrameData;

  GPUPixelContext::getInstance()->capturedFrameData = 0;
  GPUPixelContext::getInstance()->captureWidth = 0;
  GPUPixelContext::getInstance()->captureHeight = 0;
  GPUPixelContext::getInstance()->isCapturingFrame = false;
  GPUPixelContext::getInstance()->captureUpToFilter.reset();
        
  return processedFrameData;
}

void Source::setFramebuffer(
    std::shared_ptr<GPUPixelFramebuffer> fb,
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

  return _face_detector->RegCallback(callback);
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

std::shared_ptr<GPUPixelFramebuffer> Source::getFramebuffer() const {
  return _framebuffer;
}

void Source::releaseFramebuffer(bool returnToCache /* = true*/) {}

}
