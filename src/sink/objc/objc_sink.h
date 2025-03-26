/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#ifndef IOSTarget_hpp
#define IOSTarget_hpp

#import "gpupixel_sink.h"

namespace gpupixel {

class ObjcSink : public Sink {
 public:
  ObjcSink(id<GPUPixelSink> gpuSink) { _gpuSink = gpuSink; }

  virtual ~ObjcSink() { _gpuSink = 0; }

  virtual void render() override {
    [_gpuSink doRender];
  };

  virtual void setInputFramebuffer(std::shared_ptr<Framebuffer> framebuffer,
                                   RotationMode rotationMode = NoRotation,
                                   int texIdx = 0) override {
    [_gpuSink setInputFramebuffer:framebuffer
                        withRotation:rotationMode
                             atIndex:texIdx];
  };

  virtual bool isPrepared() const override {
    if ([_gpuSink respondsToSelector:@selector(isPrepared)]) {
      return [_gpuSink isPrepared];
    } else {
      return true;
    }
  }

  virtual void unPrepear() override {
    if ([_gpuSink respondsToSelector:@selector(unPrepared)]) {
      [_gpuSink unPrepared];
    }
  }

 private:
  id<GPUPixelSink> _gpuSink;
};

}

#endif  // IOSTarget_hpp
