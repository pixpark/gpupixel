/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#ifndef IOSTarget_hpp
#define IOSTarget_hpp

#import "gpupixel/sink/objc/gpupixel_sink.h"

namespace gpupixel {

class ObjcSink : public Sink {
 public:
  ObjcSink(id<GPUPixelSink> gpuSink) { _gpuSink = gpuSink; }

  virtual ~ObjcSink() { _gpuSink = 0; }

  virtual void Render() override { [_gpuSink DoRender]; };

  virtual void SetInputFramebuffer(
      std::shared_ptr<GPUPixelFramebuffer> framebuffer,
      RotationMode rotation_mode = NoRotation,
      int texIdx = 0) override {
    [_gpuSink SetInputFramebuffer:framebuffer
                     withRotation:rotation_mode
                          atIndex:texIdx];
  };

  virtual bool IsReady() const override {
    if ([_gpuSink respondsToSelector:@selector(IsReady)]) {
      return [_gpuSink IsReady];
    } else {
      return true;
    }
  }

  virtual void ResetAndClean() override {
    if ([_gpuSink respondsToSelector:@selector(unPrepared)]) {
      [_gpuSink unPrepared];
    }
  }

 private:
  id<GPUPixelSink> _gpuSink;
};

}  // namespace gpupixel

#endif  // IOSTarget_hpp
