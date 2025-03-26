/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "sink.h"
#include "util.h"

namespace gpupixel {

Sink::Sink(int inputNumber /* = 1*/) : _inputNum(inputNumber) {}

Sink::~Sink() {
  for (auto it = _inputFramebuffers.begin(); it != _inputFramebuffers.end();
       ++it) {
    if (it->second.frameBuffer) {
      it->second.frameBuffer.reset();
      it->second.frameBuffer = 0;
    }
  }
  _inputFramebuffers.clear();
}

void Sink::setInputFramebuffer(std::shared_ptr<Framebuffer> framebuffer,
                                 RotationMode rotationMode /* = NoRotation*/,
                                 int texIdx /* = 0*/) {
  InputFrameBufferInfo inputFrameBufferInfo;
  inputFrameBufferInfo.frameBuffer = framebuffer;
  inputFrameBufferInfo.rotationMode = rotationMode;
  inputFrameBufferInfo.texIndex = texIdx;
  inputFrameBufferInfo.ignoreForPrepare = false;
  _inputFramebuffers[texIdx] = inputFrameBufferInfo;
}

int Sink::getNextAvailableTextureIndex() const {
  for (int i = 0; i < _inputNum; ++i) {
    if (_inputFramebuffers.find(i) == _inputFramebuffers.end()) {
      return i;
    }
  }
  return _inputNum - 1;
}

bool Sink::isPrepared() const {
  int preparedNum = 0;
  int ignoreForPrepareNum = 0;
  for (std::map<int, InputFrameBufferInfo>::const_iterator it =
           _inputFramebuffers.begin();
       it != _inputFramebuffers.end(); ++it) {
    if (it->second.ignoreForPrepare) {
      ignoreForPrepareNum++;
    } else if (it->second.frameBuffer) {
      preparedNum++;
    }
  }
  if (ignoreForPrepareNum + preparedNum >= _inputNum) {
    return true;
  } else {
    return false;
  }
}

void Sink::unPrepear() {
  for (auto it = _inputFramebuffers.begin(); it != _inputFramebuffers.end();
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
