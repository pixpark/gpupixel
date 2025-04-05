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
    : framebuffer_(0),
      output_rotation_(RotationMode::NoRotation),
      framebuffer_scale_(1.0) {}

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
    sinks_[sink] = texIdx;
    sink->SetInputFramebuffer(framebuffer_, RotationMode::NoRotation, texIdx);
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
  if (sinks_.find(sink) != sinks_.end()) {
    return true;
  } else {
    return false;
  }
}

void Source::RemoveSink(std::shared_ptr<Sink> sink) {
  auto itr = sinks_.find(sink);
  if (itr != sinks_.end()) {
    sinks_.erase(itr);
  }
}

void Source::RemoveAllSinks() {
  sinks_.clear();
}

bool Source::DoRender(bool updateSinks) {
  if (updateSinks) {
    DoUpdateSinks();
  }
  return true;
}

void Source::DoUpdateSinks() {
  for (auto& it : sinks_) {
    auto sink = it.first;
    sink->SetInputFramebuffer(framebuffer_, output_rotation_, sinks_[sink]);
    if (sink->IsReady()) {
      sink->Render();
      sink->ResetAndClean();
    }
  }
}

unsigned char* Source::GetProcessedFrameData(std::shared_ptr<Filter> upToFilter,
                                             int width /* = 0*/,
                                             int height /* = 0*/) {
  if (GPUPixelContext::GetInstance()->is_capturing_frame_) {
    return 0;
  }

  if (width <= 0 || height <= 0) {
    if (!framebuffer_) {
      return 0;
    }
    width = GetRotatedFramebufferWidth();
    height = GetRotatedFramebufferHeight();
  }

  GPUPixelContext::GetInstance()->is_capturing_frame_ = true;
  GPUPixelContext::GetInstance()->capture_width_ = width;
  GPUPixelContext::GetInstance()->capture_height_ = height;
  GPUPixelContext::GetInstance()->capture_frame_filter_ = upToFilter;
  gpupixel::GPUPixelContext::GetInstance()->SyncRunWithContext(
      [&] { DoRender(true); });
  unsigned char* processedFrameData =
      GPUPixelContext::GetInstance()->capture_frame_data_;

  GPUPixelContext::GetInstance()->capture_frame_data_ = 0;
  GPUPixelContext::GetInstance()->capture_width_ = 0;
  GPUPixelContext::GetInstance()->capture_height_ = 0;
  GPUPixelContext::GetInstance()->is_capturing_frame_ = false;
  GPUPixelContext::GetInstance()->capture_frame_filter_.reset();

  return processedFrameData;
}

void Source::SetFramebuffer(
    std::shared_ptr<GPUPixelFramebuffer> fb,
    RotationMode outputRotation /* = RotationMode::NoRotation*/) {
  framebuffer_ = fb;
  output_rotation_ = outputRotation;
}

int Source::GetRotatedFramebufferWidth() const {
  if (framebuffer_) {
    if (rotationSwapsSize(output_rotation_)) {
      return framebuffer_->GetHeight();
    } else {
      return framebuffer_->GetWidth();
    }
  } else {
    return 0;
  }
}

int Source::GetRotatedFramebufferHeight() const {
  if (framebuffer_) {
    if (rotationSwapsSize(output_rotation_)) {
      return framebuffer_->GetWidth();
    } else {
      return framebuffer_->GetHeight();
    }
  } else {
    return 0;
  }
}

std::shared_ptr<GPUPixelFramebuffer> Source::GetFramebuffer() const {
  return framebuffer_;
}

void Source::ReleaseFramebuffer(bool returnToCache /* = true*/) {}

}  // namespace gpupixel
