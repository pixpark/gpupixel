/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include <functional>
#include <map>
#include "gpupixel_define.h"
#include "sink.h"
#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_MAC)
#import "gpupixel_sink.h"
#endif

namespace gpupixel {
class GPUPIXEL_API Filter;

class GPUPIXEL_API Source {
 public:
  Source();
  virtual ~Source();
  virtual std::shared_ptr<Source> AddSink(std::shared_ptr<Sink> sink);
  virtual std::shared_ptr<Source> AddSink(std::shared_ptr<Sink> sink,
                                          int texIdx);
#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_MAC)
  virtual std::shared_ptr<Source> AddSink(id<GPUPixelSink> sink);
#endif
  virtual void RemoveSink(std::shared_ptr<Sink> sink);
  virtual void RemoveAllSinks();
  virtual bool HasSink(const std::shared_ptr<Sink> sink) const;
  virtual std::map<std::shared_ptr<Sink>, int>& GetSinks() { return sinks_; };

  virtual void SetFramebuffer(
      std::shared_ptr<GPUPixelFramebuffer> fb,
      RotationMode outputRotation = RotationMode::NoRotation);
  virtual std::shared_ptr<GPUPixelFramebuffer> GetFramebuffer() const;
  virtual void ReleaseFramebuffer(bool returnToCache = true);

  void SetFramebufferScale(float framebufferScale) {
    framebuffer_scale_ = framebufferScale;
  }
  int GetRotatedFramebufferWidth() const;
  int GetRotatedFramebufferHeight() const;

  virtual bool DoRender(bool updateSinks = true);
  virtual void DoUpdateSinks();
 protected:
  std::shared_ptr<GPUPixelFramebuffer> framebuffer_;
  RotationMode output_rotation_;
  std::map<std::shared_ptr<Sink>, int> sinks_;
  float framebuffer_scale_;
};

}  // namespace gpupixel
