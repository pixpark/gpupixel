/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include <map>
#include <string>
#include "gpupixel_define.h"
#include "gpupixel_framebuffer.h"

namespace gpupixel {
class GPUPIXEL_API FramebufferFactory {
 public:
  FramebufferFactory();
  ~FramebufferFactory();
  std::shared_ptr<GPUPixelFramebuffer> CreateFramebuffer(
      int width,
      int height,
      bool only_texture = false,
      const TextureAttributes texture_attributes =
          GPUPixelFramebuffer::default_texture_attributes);

  void Clean();

 private:
  std::string GenerateUuid(int width,
                           int height,
                           bool only_texture,
                           const TextureAttributes texture_attributes) const;
  std::shared_ptr<GPUPixelFramebuffer> GetFramebufferByUuid(
      const std::string& hash);

  std::map<std::string, std::shared_ptr<GPUPixelFramebuffer>> framebuffers_;
  std::map<std::string, int> framebuffer_type_counts_;
};

}  // namespace gpupixel
