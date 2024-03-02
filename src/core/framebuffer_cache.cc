/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "framebuffer_cache.h"
#include "util.h"

NS_GPUPIXEL_BEGIN

FramebufferCache::FramebufferCache() {}

FramebufferCache::~FramebufferCache() {
  purge();
}

std::shared_ptr<Framebuffer> FramebufferCache::fetchFramebuffer(
    int width,
    int height,
    bool onlyTexture /* = false*/,
    const TextureAttributes textureAttributes /* = defaultTextureAttribure*/) {
  std::shared_ptr<Framebuffer> framebufferFromCache;
  std::string lookupHash =
      _getHash(width, height, onlyTexture, textureAttributes);
  int numberOfMatchingFramebuffers = 0;
  if (_framebufferTypeCounts.find(lookupHash) != _framebufferTypeCounts.end()) {
    numberOfMatchingFramebuffers = _framebufferTypeCounts[lookupHash];
  }
  if (numberOfMatchingFramebuffers < 1) {
    framebufferFromCache = std::shared_ptr<Framebuffer>(
        new Framebuffer(width, height, onlyTexture, textureAttributes));
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
      framebufferFromCache = std::shared_ptr<Framebuffer>(
          new Framebuffer(width, height, onlyTexture, textureAttributes));
    }
  }

  return framebufferFromCache;
}

void FramebufferCache::returnFramebuffer(
    std::shared_ptr<Framebuffer> framebuffer) {
  if (framebuffer == 0) {
    return;
  }
  int width = framebuffer->getWidth();
  int height = framebuffer->getHeight();
  const TextureAttributes& textureAttributes =
      framebuffer->getTextureAttributes();
  std::string lookupHash = _getHash(
      width, height, !framebuffer->hasFramebuffer(), textureAttributes);
  int numberOfMatchingFramebuffers = 0;
  if (_framebufferTypeCounts.find(lookupHash) != _framebufferTypeCounts.end()) {
    numberOfMatchingFramebuffers = _framebufferTypeCounts[lookupHash];
  }
  std::string framebufferHash = Util::str_format("%s-%ld", lookupHash.c_str(),
                                                 numberOfMatchingFramebuffers);
  _framebuffers[framebufferHash] = framebuffer;
  _framebufferTypeCounts[lookupHash] = numberOfMatchingFramebuffers + 1;
}

std::string FramebufferCache::_getHash(
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

std::shared_ptr<Framebuffer> FramebufferCache::_getFramebufferByHash(
    const std::string& hash) {
  return _framebuffers[hash];
}

void FramebufferCache::purge() {
  _framebuffers.clear();
  _framebufferTypeCounts.clear();
}

NS_GPUPIXEL_END
