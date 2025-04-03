/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include <functional>
#include <map>
#include "gpupixel_macros.h"
#include "sink.h"
#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_MAC)
#import "gpupixel_sink.h"
#endif
#include "face_detector.h"

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
  virtual std::map<std::shared_ptr<Sink>, int>& GetSinks() {
    return _sinks;
  };

  virtual void SetFramebuffer(
      std::shared_ptr<GPUPixelFramebuffer> fb,
      RotationMode outputRotation = RotationMode::NoRotation);
  virtual std::shared_ptr<GPUPixelFramebuffer> GetFramebuffer() const;
  virtual void ReleaseFramebuffer(bool returnToCache = true);

  void SetFramebufferScale(float framebufferScale) {
    _framebufferScale = framebufferScale;
  }
  int GetRotatedFramebufferWidth() const;
  int GetRotatedFramebufferHeight() const;

  virtual bool DoRender(bool updateSinks = true);
  virtual void DoUpdateSinks();

  virtual unsigned char* GetProcessedFrameData(
      std::shared_ptr<Filter> upToFilter,
      int width = 0,
      int height = 0);
  int RegLandmarkCallback(FaceDetectorCallback callback);
 protected:
  std::shared_ptr<GPUPixelFramebuffer> _framebuffer;
  RotationMode _outputRotation;
  std::map<std::shared_ptr<Sink>, int> _sinks;
  float _framebufferScale;
  std::shared_ptr<FaceDetector> _face_detector;
};

}
