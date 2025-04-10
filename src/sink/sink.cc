/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "gpupixel/sink/sink.h"
#include "utils/util.h"

namespace gpupixel {

Sink::Sink(int input_number /* = 1*/) : input_count_(input_number) {}

Sink::~Sink() {
  for (auto it = input_framebuffers_.begin(); it != input_framebuffers_.end();
       ++it) {
    if (it->second.frame_buffer) {
      it->second.frame_buffer.reset();
      it->second.frame_buffer = 0;
    }
  }
  input_framebuffers_.clear();
}

void Sink::SetInputFramebuffer(std::shared_ptr<GPUPixelFramebuffer> framebuffer,
                               RotationMode rotation_mode /* = NoRotation*/,
                               int tex_idx /* = 0*/) {
  InputFrameBufferInfo input_frame_buffer_info;
  input_frame_buffer_info.frame_buffer = framebuffer;
  input_frame_buffer_info.rotation_mode = rotation_mode;
  input_frame_buffer_info.tex_index = tex_idx;
  input_frame_buffer_info.ignore_for_prepare = false;
  input_framebuffers_[tex_idx] = input_frame_buffer_info;
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
  int prepared_num = 0;
  int ignore_for_prepare_num = 0;
  for (std::map<int, InputFrameBufferInfo>::const_iterator it =
           input_framebuffers_.begin();
       it != input_framebuffers_.end(); ++it) {
    if (it->second.ignore_for_prepare) {
      ignore_for_prepare_num++;
    } else if (it->second.frame_buffer) {
      prepared_num++;
    }
  }
  if (ignore_for_prepare_num + prepared_num >= input_count_) {
    return true;
  } else {
    return false;
  }
}

void Sink::ResetAndClean() {
  for (auto it = input_framebuffers_.begin(); it != input_framebuffers_.end();
       ++it) {
    if (!it->second.ignore_for_prepare) {
      if (it->second.frame_buffer) {
        it->second.frame_buffer.reset();
        it->second.frame_buffer = 0;
      }
    }
  }
}

}  // namespace gpupixel
