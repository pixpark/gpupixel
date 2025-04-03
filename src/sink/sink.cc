/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "sink.h"
#include "util.h"

namespace gpupixel {

Sink::Sink(int inputNumber /* = 1*/) : input_count_(inputNumber) {}

Sink::~Sink() {
  for (auto it = input_framebuffers_.begin(); it != input_framebuffers_.end();
       ++it) {
    if (it->second.frameBuffer) {
      it->second.frameBuffer.reset();
      it->second.frameBuffer = 0;
    }
  }
  input_framebuffers_.clear();
}

void Sink::SetInputFramebuffer(std::shared_ptr<GPUPixelFramebuffer> framebuffer,
                                 RotationMode rotationMode /* = NoRotation*/,
                                 int texIdx /* = 0*/) {
  InputFrameBufferInfo inputFrameBufferInfo;
  inputFrameBufferInfo.frameBuffer = framebuffer;
  inputFrameBufferInfo.rotationMode = rotationMode;
  inputFrameBufferInfo.texIndex = texIdx;
  inputFrameBufferInfo.ignoreForPrepare = false;
  input_framebuffers_[texIdx] = inputFrameBufferInfo;
}

int Sink::NextAvailableTextureIndex() const {
  for (int i = 0; i < input_count_; ++i) {
    if (input_framebuffers_.find(i) == input_framebuffers_.end()) {
      return i;
    }
  }
  return input_count_ - 1;
}

bool Sink::IsReady() const {
  int preparedNum = 0;
  int ignoreForPrepareNum = 0;
  for (std::map<int, InputFrameBufferInfo>::const_iterator it =
           input_framebuffers_.begin();
       it != input_framebuffers_.end(); ++it) {
    if (it->second.ignoreForPrepare) {
      ignoreForPrepareNum++;
    } else if (it->second.frameBuffer) {
      preparedNum++;
    }
  }
  if (ignoreForPrepareNum + preparedNum >= input_count_) {
    return true;
  } else {
    return false;
  }
}

void Sink::ResetAndClean() {
  for (auto it = input_framebuffers_.begin(); it != input_framebuffers_.end();
       ++it) {
    if (!it->second.ignoreForPrepare) {
      if (it->second.frameBuffer) {
        it->second.frameBuffer.reset();
        it->second.frameBuffer = 0;
      }
    }
  }
}

}
