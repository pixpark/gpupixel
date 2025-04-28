/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include <iostream>
#include <map>
#include "gpupixel/gpupixel_define.h"
namespace gpupixel {
enum GPUPIXEL_API RotationMode {
  NoRotation = 0,
  RotateLeft,
  RotateRight,
  FlipVertical,
  FlipHorizontal,
  RotateRightFlipVertical,
  RotateRightFlipHorizontal,
  Rotate180
};

// Forward declaration
class GPUPixelFramebuffer;

class GPUPIXEL_API Sink {
 public:
  Sink(int input_number = 1);
  virtual ~Sink();
  virtual void SetInputFramebuffer(
      std::shared_ptr<GPUPixelFramebuffer> framebuffer,
      RotationMode rotation_mode = NoRotation,
      int tex_idx = 0);

  virtual bool IsReady() const;
  virtual void ResetAndClean();
  virtual void Render() {};
  virtual int NextAvailableTextureIndex() const;
  // virtual void SetInputSizeWithIdx(int width, int height, int texture_idx)
  // {};
 protected:
  struct InputFrameBufferInfo {
    std::shared_ptr<GPUPixelFramebuffer> frame_buffer;
    RotationMode rotation_mode;
    int tex_index;
    bool ignore_for_prepare;
  };

  std::map<int, InputFrameBufferInfo> input_framebuffers_;
  int input_count_;
};

}  // namespace gpupixel
