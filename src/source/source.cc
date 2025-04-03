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
  RemoveAllSinks();
}

std::shared_ptr<Source> Source::AddSink(std::shared_ptr<Sink> sink) {
  int sinkTexIdx = sink->NextAvailableTextureIndex();
  return AddSink(sink, sinkTexIdx);
}

std::shared_ptr<Source> Source::AddSink(std::shared_ptr<Sink> sink,
                                          int texIdx) {
  if (!HasSink(sink)) {
    _sinks[sink] = texIdx;
    sink->SetInputFramebuffer(_framebuffer, RotationMode::NoRotation, texIdx);
  }
  return std::dynamic_pointer_cast<Source>(sink);
}

#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_MAC)
std::shared_ptr<Source> Source::AddSink(id<GPUPixelSink> sink) {
  auto ios_sink = std::shared_ptr<Sink>(new ObjcSink(sink));
  AddSink(ios_sink);
  return 0;
}
#endif

bool Source::HasSink(const std::shared_ptr<Sink> sink) const {
  if (_sinks.find(sink) != _sinks.end()) {
    return true;
  } else {
    return false;
  }
}

void Source::RemoveSink(std::shared_ptr<Sink> sink) {
  auto itr = _sinks.find(sink);
  if (itr != _sinks.end()) {
    _sinks.erase(itr);
  }
}

void Source::RemoveAllSinks() {
  _sinks.clear();
}

bool Source::DoRender(bool updateSinks) {
  if (updateSinks) {
    DoUpdateSinks();
  }
  return true;
}

void Source::DoUpdateSinks() {
  for (auto& it : _sinks) {
    auto sink = it.first;
    sink->SetInputFramebuffer(_framebuffer, _outputRotation,
                                _sinks[sink]);
    if (sink->IsReady()) {
      sink->Render();
      sink->ResetAndClean();
    }
  }
}

unsigned char* Source::GetProcessedFrameData(
    std::shared_ptr<Filter> upToFilter,
    int width /* = 0*/,
    int height /* = 0*/) {
  if (GPUPixelContext::GetInstance()->isCapturingFrame) {
    return 0;
  }

  if (width <= 0 || height <= 0) {
    if (!_framebuffer) {
      return 0;
    }
    width =GetRotatedFramebufferWidth();
    height = GetRotatedFramebufferHeight();
  }

  GPUPixelContext::GetInstance()->isCapturingFrame = true;
  GPUPixelContext::GetInstance()->captureWidth = width;
  GPUPixelContext::GetInstance()->captureHeight = height;
  GPUPixelContext::GetInstance()->captureUpToFilter = upToFilter;

  DoRender(true);
  unsigned char* processedFrameData =
      GPUPixelContext::GetInstance()->capturedFrameData;

  GPUPixelContext::GetInstance()->capturedFrameData = 0;
  GPUPixelContext::GetInstance()->captureWidth = 0;
  GPUPixelContext::GetInstance()->captureHeight = 0;
  GPUPixelContext::GetInstance()->isCapturingFrame = false;
  GPUPixelContext::GetInstance()->captureUpToFilter.reset();
        
  return processedFrameData;
}

void Source::SetFramebuffer(
    std::shared_ptr<GPUPixelFramebuffer> fb,
    RotationMode outputRotation /* = RotationMode::NoRotation*/) {
  _framebuffer = fb;
  _outputRotation = outputRotation;
}

int Source::GetRotatedFramebufferWidth() const {
  if (_framebuffer) {
    if (rotationSwapsSize(_outputRotation)) {
      return _framebuffer->GetHeight();
    } else {
      return _framebuffer->GetWidth();
    }
  } else {
    return 0;
  }
}

int Source::GetRotatedFramebufferHeight() const {
  if (_framebuffer) {
    if (rotationSwapsSize(_outputRotation)) {
      return _framebuffer->GetWidth();
    } else {
      return _framebuffer->GetHeight();
    }
  } else {
    return 0;
  }
}

std::shared_ptr<GPUPixelFramebuffer> Source::GetFramebuffer() const {
  return _framebuffer;
}

void Source::ReleaseFramebuffer(bool returnToCache /* = true*/) {}

}
