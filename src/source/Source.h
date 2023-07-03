/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include <functional>
#include <map>
#include "GPUPixelDef.h"
#include "Target.h"
#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_MAC)
#import "GPUPixelTarget.h"
#endif
NS_GPUPIXEL_BEGIN

class Filter;
class Source {
 public:
  Source();
  virtual ~Source();
  virtual std::shared_ptr<Source> addTarget(std::shared_ptr<Target> target);
  virtual std::shared_ptr<Source> addTarget(std::shared_ptr<Target> target,
                                            int texIdx);
#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_MAC)
  virtual std::shared_ptr<Source> addTarget(id<GPUPixelTarget> target);
#endif
  virtual void removeTarget(std::shared_ptr<Target> target);
  virtual void removeAllTargets();
  virtual bool hasTarget(const std::shared_ptr<Target> target) const;
  virtual std::map<std::shared_ptr<Target>, int>& getTargets() {
    return _targets;
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

  virtual bool proceed(bool bUpdateTargets = true, int64_t frameTime = 0);
  virtual void updateTargets(int64_t frameTime);

  virtual unsigned char* captureAProcessedFrameData(
      std::shared_ptr<Filter> upToFilter,
      int width = 0,
      int height = 0);

 protected:
  std::shared_ptr<Framebuffer> _framebuffer;
  RotationMode _outputRotation;
  std::map<std::shared_ptr<Target>, int> _targets;
  float _framebufferScale;
};

NS_GPUPIXEL_END
