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
  clean();
}

std::shared_ptr<GPUPixelFramebuffer> FramebufferFactory::fetchFramebuffer(
    int width,
    int height,
    bool onlyTexture /* = false*/,
    const TextureAttributes textureAttributes /* = defaultTextureAttribure*/) {
  std::shared_ptr<GPUPixelFramebuffer> framebufferFromCache;
  std::string lookupHash =
      uuid(width, height, onlyTexture, textureAttributes);
  int numberOfMatchingFramebuffers = 0;
  if (_framebufferTypeCounts.find(lookupHash) != _framebufferTypeCounts.end()) {
    numberOfMatchingFramebuffers = _framebufferTypeCounts[lookupHash];
  }
  if (numberOfMatchingFramebuffers < 1) {
    framebufferFromCache = std::shared_ptr<GPUPixelFramebuffer>(
        new GPUPixelFramebuffer(width, height, onlyTexture, textureAttributes));
  } else {
    int curFramebufferId = numberOfMatchingFramebuffers - 1;
    while (!framebufferFromCache && curFramebufferId >= 0) {
      std::string framebufferHash =
          Util::str_format("%s-%ld", lookupHash.c_str(), curFramebufferId);
      if (_framebuffers.find(framebufferHash) != _framebuffers.end()) {
        framebufferFromCache = _framebuffers[framebufferHash];
        _framebuffers.erase(framebufferHash);
      } else {
        framebufferFromCache = 0;
      }
      curFramebufferId--;
    }
    curFramebufferId++;
    _framebufferTypeCounts[lookupHash] = curFramebufferId;

    if (!framebufferFromCache) {
      framebufferFromCache = std::shared_ptr<GPUPixelFramebuffer>(
          new GPUPixelFramebuffer(width, height, onlyTexture, textureAttributes));
    }
  }

  return framebufferFromCache;
}
 
std::string FramebufferFactory::uuid(
    int width,
    int height,
    bool onlyTexture,
    const TextureAttributes textureAttributes) const {
  if (onlyTexture) {
    return Util::str_format("%.1dx%.1d-%d:%d:%d:%d:%d:%d:%d-NOFB", width,
                            height, textureAttributes.minFilter,
                            textureAttributes.magFilter,
                            textureAttributes.wrapS, textureAttributes.wrapT,
                            textureAttributes.internalFormat,
                            textureAttributes.format, textureAttributes.type);
  } else {
    return Util::str_format("%.1dx%.1d-%d:%d:%d:%d:%d:%d:%d", width, height,
                            textureAttributes.minFilter,
                            textureAttributes.magFilter,
                            textureAttributes.wrapS, textureAttributes.wrapT,
                            textureAttributes.internalFormat,
                            textureAttributes.format, textureAttributes.type);
  }
}

std::shared_ptr<GPUPixelFramebuffer> FramebufferFactory::_getFramebufferByUuid(
    const std::string& hash) {
  return _framebuffers[hash];
}

void FramebufferFactory::clean() {
  _framebuffers.clear();
  _framebufferTypeCounts.clear();
}

}
