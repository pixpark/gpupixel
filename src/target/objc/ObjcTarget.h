/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#ifndef IOSTarget_hpp
#define IOSTarget_hpp

#import "GPUPixelTarget.h"

NS_GPUPIXEL_BEGIN

class ObjcTarget : public Target {
 public:
  ObjcTarget(id<GPUPixelTarget> realTarget) { _realTarget = realTarget; }

  virtual ~ObjcTarget() { _realTarget = 0; }

  virtual void update(int64_t frameTime) override {
    [_realTarget update:frameTime];
  };

  virtual void setInputFramebuffer(std::shared_ptr<Framebuffer> framebuffer,
                                   RotationMode rotationMode = NoRotation,
                                   int texIdx = 0) override {
    [_realTarget setInputFramebuffer:framebuffer
                        withRotation:rotationMode
                             atIndex:texIdx];
  };

  virtual bool isPrepared() const override {
    if ([_realTarget respondsToSelector:@selector(isPrepared)]) {
      return [_realTarget isPrepared];
    } else {
      return true;
    }
  }

  virtual void unPrepear() override {
    if ([_realTarget respondsToSelector:@selector(unPrepared)]) {
      [_realTarget unPrepared];
    }
  }

 private:
  id<GPUPixelTarget> _realTarget;
};

NS_GPUPIXEL_END

#endif  // IOSTarget_hpp
