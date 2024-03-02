/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include <map>
#include <string>
#include "framebuffer.h"
#include "gpupixel_macros.h"

NS_GPUPIXEL_BEGIN

class FramebufferCache {
 public:
  FramebufferCache();
  ~FramebufferCache();
  std::shared_ptr<Framebuffer> fetchFramebuffer(
      int width,
      int height,
      bool onlyTexture = false,
      const TextureAttributes textureAttributes =
          Framebuffer::defaultTextureAttribures);
  void returnFramebuffer(std::shared_ptr<Framebuffer> framebuffer);
  void purge();

 private:
  std::string _getHash(int width,
                       int height,
                       bool onlyTexture,
                       const TextureAttributes textureAttributes) const;
  std::shared_ptr<Framebuffer> _getFramebufferByHash(const std::string& hash);

  std::map<std::string, std::shared_ptr<Framebuffer>> _framebuffers;
  std::map<std::string, int> _framebufferTypeCounts;
};

NS_GPUPIXEL_END
