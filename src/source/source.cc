/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "gpupixel/source/source.h"
#include "core/gpupixel_context.h"
#include "utils/util.h"
 
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
