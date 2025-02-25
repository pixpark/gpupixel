/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "target.h"
#include "util.h"

NS_GPUPIXEL_BEGIN

Target::Target(int inputNumber /* = 1*/) : _inputNum(inputNumber) {}

Target::~Target() {
  for (auto it = _inputFramebuffers.begin(); it != _inputFramebuffers.end();
       ++it) {
    if (it->second.frameBuffer) {
      it->second.frameBuffer.reset();
      it->second.frameBuffer = 0;
    }
  }
  _inputFramebuffers.clear();
}

void Target::setInputFramebuffer(std::shared_ptr<Framebuffer> framebuffer,
                                 RotationMode rotationMode /* = NoRotation*/,
                                 int texIdx /* = 0*/) {
  InputFrameBufferInfo inputFrameBufferInfo;
  inputFrameBufferInfo.frameBuffer = framebuffer;
  inputFrameBufferInfo.rotationMode = rotationMode;
  inputFrameBufferInfo.texIndex = texIdx;
  inputFrameBufferInfo.ignoreForPrepare = false;
  _inputFramebuffers[texIdx] = inputFrameBufferInfo;
}

int Target::getNextAvailableTextureIndex() const {
  for (int i = 0; i < _inputNum; ++i) {
    if (_inputFramebuffers.find(i) == _inputFramebuffers.end()) {
      return i;
    }
  }
  return _inputNum - 1;
}

bool Target::isPrepared() const {
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

void Target::unPrepear() {
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

#ifdef __emscripten__
EMSCRIPTEN_BINDINGS(target){
  emscripten::class_<Target>("Target")
  .constructor<int>()
  .smart_ptr<std::shared_ptr<Target>>("Target");
}
#endif
NS_GPUPIXEL_END
