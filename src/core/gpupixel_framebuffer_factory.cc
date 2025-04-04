/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "gpupixel_framebuffer_factory.h"
#include "util.h"

namespace gpupixel {

FramebufferFactory::FramebufferFactory() {}

FramebufferFactory::~FramebufferFactory() {
  Clean();
}

std::shared_ptr<GPUPixelFramebuffer> FramebufferFactory::CreateFramebuffer(
    int width,
    int height,
    bool only_texture /* = false*/,
    const TextureAttributes texture_attributes /* = defaultTextureAttribure*/) {
  std::shared_ptr<GPUPixelFramebuffer> framebuffer_from_cache;
  std::string lookup_hash =
      GenerateUuid(width, height, only_texture, texture_attributes);
  int number_of_matching_framebuffers = 0;
  if (framebuffer_type_counts_.find(lookup_hash) !=
      framebuffer_type_counts_.end()) {
    number_of_matching_framebuffers = framebuffer_type_counts_[lookup_hash];
  }
  if (number_of_matching_framebuffers < 1) {
    framebuffer_from_cache =
        std::shared_ptr<GPUPixelFramebuffer>(new GPUPixelFramebuffer(
            width, height, only_texture, texture_attributes));
  } else {
    int cur_framebuffer_id = number_of_matching_framebuffers - 1;
    while (!framebuffer_from_cache && cur_framebuffer_id >= 0) {
      std::string framebuffer_hash =
          Util::str_format("%s-%ld", lookup_hash.c_str(), cur_framebuffer_id);
      if (framebuffers_.find(framebuffer_hash) != framebuffers_.end()) {
        framebuffer_from_cache = framebuffers_[framebuffer_hash];
        framebuffers_.erase(framebuffer_hash);
      } else {
        framebuffer_from_cache = 0;
      }
      cur_framebuffer_id--;
    }
    cur_framebuffer_id++;
    framebuffer_type_counts_[lookup_hash] = cur_framebuffer_id;

    if (!framebuffer_from_cache) {
      framebuffer_from_cache =
          std::shared_ptr<GPUPixelFramebuffer>(new GPUPixelFramebuffer(
              width, height, only_texture, texture_attributes));
    }
  }

  return framebuffer_from_cache;
}

std::string FramebufferFactory::GenerateUuid(
    int width,
    int height,
    bool only_texture,
    const TextureAttributes texture_attributes) const {
  if (only_texture) {
    return Util::str_format("%.1dx%.1d-%d:%d:%d:%d:%d:%d:%d-NOFB", width,
                            height, texture_attributes.minFilter,
                            texture_attributes.magFilter,
                            texture_attributes.wrapS, texture_attributes.wrapT,
                            texture_attributes.internalFormat,
                            texture_attributes.format, texture_attributes.type);
  } else {
    return Util::str_format("%.1dx%.1d-%d:%d:%d:%d:%d:%d:%d", width, height,
                            texture_attributes.minFilter,
                            texture_attributes.magFilter,
                            texture_attributes.wrapS, texture_attributes.wrapT,
                            texture_attributes.internalFormat,
                            texture_attributes.format, texture_attributes.type);
  }
}

std::shared_ptr<GPUPixelFramebuffer> FramebufferFactory::GetFramebufferByUuid(
    const std::string& hash) {
  return framebuffers_[hash];
}

void FramebufferFactory::Clean() {
  framebuffers_.clear();
  framebuffer_type_counts_.clear();
}

}  // namespace gpupixel
