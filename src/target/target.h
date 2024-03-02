/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "framebuffer.h"
#include "gpupixel_macros.h"

#include <map>

NS_GPUPIXEL_BEGIN

enum RotationMode {
  NoRotation = 0,
  RotateLeft,
  RotateRight,
  FlipVertical,
  FlipHorizontal,
  RotateRightFlipVertical,
  RotateRightFlipHorizontal,
  Rotate180
};

class Target {
 public:
  Target(int inputNumber = 1);
  virtual ~Target();
  virtual void setInputFramebuffer(std::shared_ptr<Framebuffer> framebuffer,
                                   RotationMode rotationMode = NoRotation,
                                   int texIdx = 0);

  virtual bool isPrepared() const;
  virtual void unPrepear();
  virtual void update(int64_t frameTime){};
  virtual int getNextAvailableTextureIndex() const;
  // virtual void setInputSizeWithIdx(int width, int height, int textureIdx) {};
 protected:
  struct InputFrameBufferInfo {
    std::shared_ptr<Framebuffer> frameBuffer;
    RotationMode rotationMode;
    int texIndex;
    bool ignoreForPrepare;
  };

  std::map<int, InputFrameBufferInfo> _inputFramebuffers;
  int _inputNum;
};

NS_GPUPIXEL_END
