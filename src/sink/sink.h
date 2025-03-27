/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "gpupixel_framebuffer.h"
#include "gpupixel_macros.h"

#include <map>

namespace gpupixel {
GPUPIXEL_API enum RotationMode {
  NoRotation = 0,
  RotateLeft,
  RotateRight,
  FlipVertical,
  FlipHorizontal,
  RotateRightFlipVertical,
  RotateRightFlipHorizontal,
  Rotate180
};

class GPUPIXEL_API Sink {
 public:
  Sink(int inputNumber = 1);
  virtual ~Sink();
  virtual void setInputFramebuffer(std::shared_ptr<GPUPixelFramebuffer> framebuffer,
                                   RotationMode rotationMode = NoRotation,
                                   int texIdx = 0);

  virtual bool isPrepared() const;
  virtual void unPrepear();
  virtual void render(){};
  virtual int getNextAvailableTextureIndex() const;
  // virtual void setInputSizeWithIdx(int width, int height, int textureIdx) {};
 protected:
  struct InputFrameBufferInfo {
    std::shared_ptr<GPUPixelFramebuffer> frameBuffer;
    RotationMode rotationMode;
    int texIndex;
    bool ignoreForPrepare;
  };

  std::map<int, InputFrameBufferInfo> _inputFramebuffers;
  int _inputNum;
};

}
