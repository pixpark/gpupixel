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
  virtual std::shared_ptr<Source> addSink(std::shared_ptr<Sink> sink);
  virtual std::shared_ptr<Source> addSink(std::shared_ptr<Sink> sink,
                                            int texIdx);
#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_MAC)
  virtual std::shared_ptr<Source> addSink(id<GPUPixelSink> sink);
#endif
  virtual void removeSink(std::shared_ptr<Sink> sink);
  virtual void removeAllSinks();
  virtual bool hasSink(const std::shared_ptr<Sink> sink) const;
  virtual std::map<std::shared_ptr<Sink>, int>& getSinks() {
    return _sinks;
  };

  virtual void setFramebuffer(
      std::shared_ptr<Framebuffer> fb,
      RotationMode outputRotation = RotationMode::NoRotation);
  virtual std::shared_ptr<Framebuffer> getFramebuffer() const;
  virtual void releaseFramebuffer(bool returnToCache = true);

  void setFramebufferScale(float framebufferScale) {
    _framebufferScale = framebufferScale;
  }
  int getRotatedFramebufferWidth() const;
  int getRotatedFramebufferHeight() const;

  virtual bool doRender(bool updateSinks = true);
  virtual void doUpdateSinks();

  virtual unsigned char* captureAProcessedFrameData(
      std::shared_ptr<Filter> upToFilter,
      int width = 0,
      int height = 0);
  int RegLandmarkCallback(FaceDetectorCallback callback);
 protected:
  std::shared_ptr<Framebuffer> _framebuffer;
  RotationMode _outputRotation;
  std::map<std::shared_ptr<Sink>, int> _sinks;
  float _framebufferScale;
  std::shared_ptr<FaceDetector> _face_detector;
};

}
