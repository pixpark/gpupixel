/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include <map>
#include <string>
#include "gpupixel_framebuffer.h"
#include "gpupixel_define.h"

namespace gpupixel {
class GPUPIXEL_API FramebufferFactory {
 public:
  FramebufferFactory();
  ~FramebufferFactory();
  std::shared_ptr<GPUPixelFramebuffer> CreateFramebuffer(
      int width,
      int height,
      bool onlyTexture = false,
      const TextureAttributes textureAttributes =
          GPUPixelFramebuffer::defaultTextureAttribures);

  void Clean();

 private:
  std::string uuid(int width,
                       int height,
                       bool onlyTexture,
                       const TextureAttributes textureAttributes) const;
  std::shared_ptr<GPUPixelFramebuffer> _getFramebufferByUuid(const std::string& hash);

  std::map<std::string, std::shared_ptr<GPUPixelFramebuffer>> _framebuffers;
  std::map<std::string, int> _framebufferTypeCounts;
};

}
